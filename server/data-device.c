#include "data-device.h"

#include <zmonitors-util.h>

#include "data-offer.h"
#include "data-source.h"
#include "output.h"
#include "seat.h"
#include "surface.h"
#include "view.h"

static void
zms_data_device_handle_destroy(struct wl_resource* resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void
zms_data_device_protocol_start_drag(struct wl_client* client,
    struct wl_resource* resource, struct wl_resource* source,
    struct wl_resource* origin_resource, struct wl_resource* icon,
    uint32_t serial)
{
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(icon);
  struct zms_surface* origin = wl_resource_get_user_data(origin_resource);
  struct zms_output* output;
  struct zms_data_source* data_source = wl_resource_get_user_data(source);

  if (zms_view_is_mapped(origin->view) == false) return;

  output = origin->view->priv->output;
  output->priv->interface->start_drag(
      output->priv->user_data, data_source->proxy, serial);
}

static void
zms_data_device_protocol_set_selection(struct wl_client* client,
    struct wl_resource* resource, struct wl_resource* source, uint32_t serial)
{
  // TODO:
  zms_log("request not implemented yet: wl_data_device.set_selection\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(source);
  Z_UNUSED(serial);
}

static void
zms_data_device_protocol_release(
    struct wl_client* client, struct wl_resource* resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static const struct wl_data_device_interface data_device_interface = {
    .start_drag = zms_data_device_protocol_start_drag,
    .set_selection = zms_data_device_protocol_set_selection,
    .release = zms_data_device_protocol_release,
};

static struct wl_resource*
zms_data_device_find_resource(
    struct zms_data_device* data_device, struct wl_client* client)
{
  struct wl_resource* resource;

  wl_resource_for_each(resource, &data_device->resource_list)
  {
    if (wl_resource_get_client(resource) == client) return resource;
  }

  return NULL;
}

ZMS_EXPORT enum zms_data_device_set_focus_result
zms_data_device_set_focus(struct zms_data_device* data_device,
    struct zms_view* view /* nullable */, float vx, float vy)
{
  struct zms_view* current_focus_view = data_device->focus_view_ref.data;
  struct wl_display* display = data_device->seat->priv->compositor->display;
  struct zms_data_offer_proxy* data_offer_proxy =
      data_device->data_offer_proxy_ref.data;
  enum zms_data_device_set_focus_result res = 0;

  data_device->vx = vx;
  data_device->vy = vy;

  if (current_focus_view == view) return res;

  if (current_focus_view) {
    struct wl_resource* resource;

    resource = zms_data_device_find_resource(data_device,
        wl_resource_get_client(current_focus_view->priv->surface->resource));

    if (resource) {
      wl_data_device_send_leave(resource);
      res |= ZMS_DATA_DEVICE_SET_FOCUS_LEFT;
    }

    data_device->focus_serial = 0;
  }

  if (view && data_offer_proxy) {
    struct wl_resource* resource;
    struct wl_client* client =
        wl_resource_get_client(view->priv->surface->resource);
    uint32_t serial = wl_display_next_serial(display);

    resource = zms_data_device_find_resource(data_device, client);

    if (resource) {
      struct zms_data_offer* data_offer;
      data_offer = zms_data_offer_create(client, data_offer_proxy, data_device);
      zms_weak_reference(&data_device->data_offer_in_use_ref, data_offer,
          &data_offer->destroy_signal);

      wl_data_device_send_data_offer(resource, data_offer->resource);
      zms_data_offer_offer(data_offer);

      wl_data_device_send_enter(resource, serial, view->priv->surface->resource,
          wl_fixed_from_double(vx), wl_fixed_from_double(vy),
          data_offer->resource);
      res |= ZMS_DATA_DEVICE_SET_FOCUS_ENTERED;
    }

    data_device->focus_serial = serial;
  }

  zms_weak_reference(
      &data_device->focus_view_ref, view, view ? &view->destroy_signal : NULL);

  return res;
}

ZMS_EXPORT struct zms_data_device*
zms_data_device_create(
    struct zms_seat* seat /* not fully initialized at this point */)
{
  struct zms_data_device* data_device;

  data_device = zalloc(sizeof *data_device);
  if (data_device == NULL) goto err;

  data_device->seat = seat;
  wl_list_init(&data_device->resource_list);
  zms_weak_ref_init(&data_device->data_offer_proxy_ref);
  zms_weak_ref_init(&data_device->data_offer_in_use_ref);
  zms_weak_ref_init(&data_device->focus_view_ref);
  data_device->focus_serial = 0;
  data_device->proxy_enter_serial = 0;
  data_device->is_dragging = false;

  return data_device;

err:
  return NULL;
}

ZMS_EXPORT void
zms_data_device_destroy(struct zms_data_device* data_device)
{
  struct wl_resource *resource, *tmp;
  struct zms_data_offer_proxy* data_offer_proxy;

  wl_resource_for_each_safe(resource, tmp, &data_device->resource_list)
      wl_resource_destroy(resource);

  data_offer_proxy = data_device->data_offer_proxy_ref.data;
  if (data_offer_proxy) data_offer_proxy->destroy(data_offer_proxy);

  zms_weak_reference(&data_device->data_offer_in_use_ref, NULL, NULL);
  zms_weak_reference(&data_device->focus_view_ref, NULL, NULL);
  free(data_device);
}

ZMS_EXPORT struct wl_resource*
zms_data_device_create_resource(
    struct zms_data_device* data_device, struct wl_client* client, uint32_t id)
{
  struct wl_resource* resource;

  resource = wl_resource_create(client, &wl_data_device_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  wl_resource_set_implementation(resource, &data_device_interface, data_device,
      zms_data_device_handle_destroy);

  wl_list_insert(&data_device->resource_list, wl_resource_get_link(resource));

  return resource;

err:
  return NULL;
}

ZMS_EXPORT void
zms_data_device_send_motion(struct zms_data_device* data_device, uint32_t time)
{
  struct wl_resource* resource;
  struct zms_view* view;

  view = data_device->focus_view_ref.data;

  if (view == NULL) return;

  resource = zms_data_device_find_resource(
      data_device, wl_resource_get_client(view->priv->surface->resource));

  if (resource == NULL) return;

  wl_data_device_send_motion(resource, time,
      wl_fixed_from_double(data_device->vx),
      wl_fixed_from_double(data_device->vy));
}

/**
 * zms_data_offer_proxy objects are managed by zms_data_device, but they can be
 * destroyed outside of the zms_data_device. Up to 1 zms_data_offer_proxy object
 * can be alive is the system, and zms_data_device.data_offer_proxy_ref is the
 * reference to the zms_data_offer_proxy object.
 */
ZMS_EXPORT void
zms_data_device_notify_new_data_offer(struct zms_data_device* data_device,
    struct zms_data_offer_proxy* data_offer_proxy)
{
  struct zms_data_offer_proxy* prev_data_offer_proxy;

  prev_data_offer_proxy = data_device->data_offer_proxy_ref.data;
  if (prev_data_offer_proxy)
    prev_data_offer_proxy->destroy(prev_data_offer_proxy);
  zms_weak_reference(&data_device->data_offer_proxy_ref, data_offer_proxy,
      &data_offer_proxy->destroy_signal);
}

ZMS_EXPORT void
zms_data_device_notify_drop(struct zms_data_device* data_device)
{
  struct zms_view* focus_view = data_device->focus_view_ref.data;
  struct wl_client* client;
  struct wl_resource* data_device_target_resource;

  if (focus_view == NULL) return;

  client = wl_resource_get_client(focus_view->priv->surface->resource);

  data_device_target_resource =
      zms_data_device_find_resource(data_device, client);

  if (data_device_target_resource == NULL) return;

  wl_data_device_send_drop(data_device_target_resource);
}

ZMS_EXPORT void
zms_data_device_notify_leave(struct zms_data_device* data_device)
{
  data_device->is_dragging = false;
}

ZMS_EXPORT void
zms_data_device_destroy_data_offer(struct zms_data_device* data_device)
{
  struct zms_data_offer_proxy* data_offer_proxy;

  data_offer_proxy = data_device->data_offer_proxy_ref.data;

  if (data_offer_proxy) data_offer_proxy->destroy(data_offer_proxy);
}
