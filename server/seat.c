#include "seat.h"

#include <zmonitors-server.h>

#include "compositor.h"
#include "drag-grab.h"
#include "output.h"
#include "pointer-client.h"

static void
zms_seat_protocol_get_pointer(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  struct zms_pointer_client* pointer_client;
  struct zms_seat* seat = wl_resource_get_user_data(resource);

  if (seat->priv->pointer) {
    pointer_client = zms_pointer_client_ensure(client, seat->priv->pointer);
    zms_pointer_client_resource_create(pointer_client, client, id);
  } else {
    zms_pointer_client_inert_resource_create(client, id);
  }
}

static void
zms_seat_protocol_get_keyboard(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  // TODO:
  zms_log("request not implemented yet: wl_seat.get_keyboard\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(id);
}

static void
zms_seat_protocol_get_touch(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  // TODO:
  zms_log("request not implemented yet: wl_seat.get_touch\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(id);
}

static void
zms_seat_protocol_release(
    struct wl_client* client, struct wl_resource* resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static const struct wl_seat_interface seat_interface = {
    .get_pointer = zms_seat_protocol_get_pointer,
    .get_keyboard = zms_seat_protocol_get_keyboard,
    .get_touch = zms_seat_protocol_get_touch,
    .release = zms_seat_protocol_release,
};

static void
zms_seat_send_capabilities(
    struct zms_seat* seat, struct wl_client* client /* nullable */)
{
  struct wl_resource* resource;
  uint32_t capabilities = 0;

  if (seat->priv->pointer) capabilities |= WL_SEAT_CAPABILITY_POINTER;

  wl_resource_for_each(resource, &seat->priv->resource_list)
  {
    if (client == NULL || wl_resource_get_client(resource) == client)
      wl_seat_send_capabilities(resource, capabilities);
  }
}

static void
zms_seat_handle_destroy(struct wl_resource* resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void
zms_seat_bind(
    struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
  struct zms_seat* seat = data;
  struct wl_resource* resource;

  resource = wl_resource_create(client, &wl_seat_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(
      resource, &seat_interface, seat, zms_seat_handle_destroy);

  wl_list_insert(&seat->priv->resource_list, wl_resource_get_link(resource));

  zms_seat_send_capabilities(seat, client);
  wl_seat_send_name(resource, seat->priv->name);
}

ZMS_EXPORT struct zms_seat*
zms_seat_create(struct zms_compositor* compositor)
{
  struct zms_seat* seat;
  struct zms_seat_private* priv;
  struct wl_global* global;
  struct zms_data_device* data_device;

  seat = zalloc(sizeof seat);
  if (seat == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  priv = zalloc(sizeof *priv);
  if (priv == NULL) {
    zms_log("failed to allocate memory\n");
    goto err_priv;
  }

  data_device = zms_data_device_create(seat);
  if (data_device == NULL) {
    zms_log("failed to create a data device\n");
    goto err_data_device;
  }

  global = wl_global_create(
      compositor->display, &wl_seat_interface, 7, seat, zms_seat_bind);
  if (global == NULL) {
    zms_log("failed to create a seat wl_global\n");
    goto err_global;
  }

  priv->global = global;
  priv->compositor = compositor;
  priv->name = "default";
  wl_list_init(&priv->resource_list);

  seat->priv = priv;
  seat->data_device = data_device;

  return seat;

err_global:
  zms_data_device_destroy(data_device);

err_data_device:
  free(priv);

err_priv:
  free(seat);

err:
  return NULL;
}

ZMS_EXPORT void
zms_seat_destroy(struct zms_seat* seat)
{
  struct wl_resource *resource, *tmp;

  wl_resource_for_each_safe(resource, tmp, &seat->priv->resource_list)
  {
    wl_resource_set_destructor(resource, NULL);
    wl_resource_set_user_data(resource, NULL);
    wl_list_remove(wl_resource_get_link(resource));
  }

  zms_data_device_destroy(seat->data_device);
  wl_global_destroy(seat->priv->global);
  free(seat->priv);
  free(seat);
}

ZMS_EXPORT void
zms_seat_init_pointer(struct zms_seat* seat)
{
  if (seat->priv->pointer) return;

  seat->priv->pointer = zms_pointer_create(seat);
  zms_seat_send_capabilities(seat, NULL);
}

ZMS_EXPORT void
zms_seat_release_pointer(struct zms_seat* seat)
{
  if (seat->priv->pointer == NULL) return;
  zms_pointer_destroy(seat->priv->pointer);
  seat->priv->pointer = NULL;
  zms_seat_send_capabilities(seat, NULL);
}

ZMS_EXPORT void
zms_seat_notify_pointer_enter(
    struct zms_seat* seat, struct zms_output* output, vec2 pos)
{
  struct zms_pointer* pointer = seat->priv->pointer;

  if (pointer) zms_pointer_move_to(pointer, output, pos[0], pos[1]);
}

ZMS_EXPORT void
zms_seat_notify_pointer_motion_abs(
    struct zms_seat* seat, struct zms_output* output, vec2 pos, uint32_t time)
{
  struct zms_pointer* pointer = seat->priv->pointer;

  if (pointer)
    pointer->grab->interface->motion_abs(pointer->grab, output, time, pos);
}

ZMS_EXPORT void
zms_seat_notify_pointer_button(struct zms_seat* seat, uint32_t time,
    uint32_t button, uint32_t state, uint32_t serial)
{
  Z_UNUSED(seat);
  struct zms_pointer* pointer = seat->priv->pointer;

  if (pointer == NULL) return;

  if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
    if (pointer->button_count == 0) {
      pointer->grab_x = pointer->x;
      pointer->grab_y = pointer->y;
    }
    pointer->button_count++;
  } else if (pointer->button_count > 0) {
    pointer->button_count--;
  }

  pointer->grab->interface->button(pointer->grab, time, button, state, serial);

  if (pointer->button_count == 1) pointer->grab_serial = serial;
}

ZMS_EXPORT void
zms_seat_notify_pointer_leave(struct zms_seat* seat)
{
  struct zms_pointer* pointer = seat->priv->pointer;

  seat->data_device->proxy_enter_serial = 0;

  if (pointer == NULL) return;

  pointer->grab->interface->cancel(pointer->grab);

  zms_pointer_move_to(pointer, NULL, 0, 0);

  pointer->grab_serial = 0;
  pointer->button_count = 0;
}

ZMS_EXPORT void
zms_seat_notify_start_drag(struct zms_seat* seat, uint32_t enter_serial)
{
  zms_drag_grab_start(seat);
  seat->data_device->proxy_enter_serial = enter_serial;
  seat->data_device->is_dragging = true;
}
