#include "data-offer.h"

#include <zmonitors-util.h>

#include "data-device.h"

static void zms_data_offer_destroy(struct zms_data_offer* data_offer);

static void
zms_data_offer_handle_destroy(struct wl_resource* resource)
{
  struct zms_data_offer* data_offer;

  data_offer = wl_resource_get_user_data(resource);

  zms_data_offer_destroy(data_offer);
}

static bool
zms_data_offer_is_in_use(struct zms_data_offer* data_offer)
{
  struct zms_data_offer_proxy* data_offer_proxy;
  struct zms_data_device* data_device;

  data_device = data_offer->data_device;
  data_offer_proxy = data_offer->data_offer_proxy_ref.data;

  if (data_offer_proxy &&
      data_offer_proxy == data_device->data_offer_proxy_ref.data &&
      data_device->data_offer_in_use_ref.data == data_offer)
    return true;

  return false;
}

static void
zms_data_offer_protocol_accept(struct wl_client* client,
    struct wl_resource* resource, uint32_t serial, const char* mime_type)
{
  Z_UNUSED(client);
  struct zms_data_offer* data_offer;
  struct zms_data_offer_proxy* data_offer_proxy;
  struct zms_data_device* data_device;

  data_offer = wl_resource_get_user_data(resource);
  data_device = data_offer->data_device;
  data_offer_proxy = data_offer->data_offer_proxy_ref.data;

  if (zms_data_offer_is_in_use(data_offer) &&
      data_device->focus_serial == serial)
    data_offer_proxy->accept(
        data_offer_proxy, data_device->proxy_enter_serial, mime_type);
}

static void
zms_data_offer_protocol_receive(struct wl_client* client,
    struct wl_resource* resource, const char* mime_type, int32_t fd)
{
  Z_UNUSED(client);
  struct zms_data_offer* data_offer;
  struct zms_data_offer_proxy* data_offer_proxy;

  data_offer = wl_resource_get_user_data(resource);
  data_offer_proxy = data_offer->data_offer_proxy_ref.data;

  if (zms_data_offer_is_in_use(data_offer))
    data_offer_proxy->receive(data_offer_proxy, mime_type, fd);
}

static void
zms_data_offer_protocol_destroy(
    struct wl_client* client, struct wl_resource* resource)
{
  Z_UNUSED(client);
  struct zms_data_offer* data_offer;
  struct zms_data_offer_proxy* data_offer_proxy;
  struct zms_data_device* data_device;

  data_offer = wl_resource_get_user_data(resource);
  data_offer_proxy = data_offer->data_offer_proxy_ref.data;
  data_device = data_offer->data_device;

  if (zms_data_offer_is_in_use(data_offer) && data_device->is_dragging == false)
    data_offer_proxy->destroy(data_offer_proxy);

  wl_resource_destroy(resource);
}

static void
zms_data_offer_protocol_finish(
    struct wl_client* client, struct wl_resource* resource)
{
  Z_UNUSED(client);
  struct zms_data_offer* data_offer;
  struct zms_data_offer_proxy* data_offer_proxy;

  data_offer = wl_resource_get_user_data(resource);
  data_offer_proxy = data_offer->data_offer_proxy_ref.data;

  if (zms_data_offer_is_in_use(data_offer))
    data_offer_proxy->finish(data_offer_proxy);
}

static void
zms_data_offer_protocol_set_actions(struct wl_client* client,
    struct wl_resource* resource, uint32_t dnd_actions,
    uint32_t preferred_action)
{
  // TODO:
  zms_log("request not implemented yet: wl_data_offer.set_action\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(dnd_actions);
  Z_UNUSED(preferred_action);
}

static const struct wl_data_offer_interface data_offer_interface = {
    .accept = zms_data_offer_protocol_accept,
    .receive = zms_data_offer_protocol_receive,
    .destroy = zms_data_offer_protocol_destroy,
    .finish = zms_data_offer_protocol_finish,
    .set_actions = zms_data_offer_protocol_set_actions,
};

ZMS_EXPORT struct zms_data_offer*
zms_data_offer_create(struct wl_client* client,
    struct zms_data_offer_proxy* data_offer_proxy,
    struct zms_data_device* data_device)
{
  struct zms_data_offer* data_offer;
  struct wl_resource* resource;

  data_offer = zalloc(sizeof *data_offer);
  if (data_offer == NULL) goto err;

  resource = wl_resource_create(client, &wl_data_offer_interface, 3, 0);
  if (resource == NULL) goto err_resource;

  wl_resource_set_implementation(resource, &data_offer_interface, data_offer,
      zms_data_offer_handle_destroy);

  zms_weak_ref_init(&data_offer->data_offer_proxy_ref);
  zms_weak_reference(&data_offer->data_offer_proxy_ref, data_offer_proxy,
      &data_offer_proxy->destroy_signal);
  data_offer->data_device = data_device;
  data_offer->resource = resource;
  zms_signal_init(&data_offer->destroy_signal);

  return data_offer;

err_resource:
  free(data_offer);

err:
  return NULL;
}

static void
zms_data_offer_destroy(struct zms_data_offer* data_offer)
{
  zms_signal_emit(&data_offer->destroy_signal, NULL);
  zms_weak_reference(&data_offer->data_offer_proxy_ref, NULL, NULL);
  free(data_offer);
}

ZMS_EXPORT void
zms_data_offer_offer(struct zms_data_offer* data_offer)
{
  struct zms_data_offer_proxy* data_offer_proxy;
  char** type;

  data_offer_proxy = data_offer->data_offer_proxy_ref.data;

  if (data_offer_proxy == NULL) return;

  wl_array_for_each(type, &data_offer_proxy->mime_types)
      wl_data_offer_send_offer(data_offer->resource, *type);
}
