#include "pointer-client.h"

#include "cursor-sprite.h"
#include "seat.h"
#include "surface.h"

static void zms_pointer_client_destroy(
    struct zms_pointer_client *pointer_client);

static void
zms_pointer_client_handle_destroy(struct wl_resource *resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void
zms_pointer_client_protocol_set_cursor(struct wl_client *client,
    struct wl_resource *resource, uint32_t serial,
    struct wl_resource *surface_resource /* nullable */, int32_t hotspot_x,
    int32_t hotspot_y)
{
  Z_UNUSED(client);
  Z_UNUSED(serial);
  struct zms_pointer_client *pointer_client =
      wl_resource_get_user_data(resource);
  struct zms_pointer *pointer;
  struct zms_surface *surface = NULL;
  struct zms_view *focus_view;

  if (pointer_client == NULL) return; /* inert resource */

  pointer = pointer_client->pointer;
  focus_view = pointer->focus_view_ref.data;

  if (focus_view == NULL) return;

  if (surface_resource) surface = wl_resource_get_user_data(surface_resource);

  if (wl_resource_get_client(focus_view->priv->surface->resource) != client)
    return;

  zms_pointer_set_cursor(pointer, surface, hotspot_x, hotspot_y);
}

static void
zms_pointer_client_protocol_release(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);

  wl_resource_destroy(resource);
}

static const struct wl_pointer_interface pointer_interface = {
    .set_cursor = zms_pointer_client_protocol_set_cursor,
    .release = zms_pointer_client_protocol_release,
};

static void
pointer_destroy_handler(struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_pointer_client *pointer_client;

  pointer_client =
      wl_container_of(listener, pointer_client, pointer_destroy_listener);

  zms_pointer_client_destroy(pointer_client);
}

static void
client_destroy_handler(struct wl_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_pointer_client *pointer_client;

  pointer_client =
      wl_container_of(listener, pointer_client, client_destroy_listener);

  zms_pointer_client_destroy(pointer_client);
}

static struct zms_pointer_client *
zms_pointer_client_create(struct wl_client *client, struct zms_pointer *pointer)
{
  struct zms_pointer_client *pointer_client;

  pointer_client = zalloc(sizeof *pointer_client);
  if (pointer_client == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  pointer_client->pointer = pointer;
  wl_list_insert(&pointer->point_client_list, &pointer_client->link);

  pointer_client->pointer_destroy_listener.notify = pointer_destroy_handler;
  zms_signal_add(
      &pointer->destroy_signal, &pointer_client->pointer_destroy_listener);

  pointer_client->client = client;
  pointer_client->client_destroy_listener.notify = client_destroy_handler;
  wl_client_add_destroy_listener(
      client, &pointer_client->client_destroy_listener);

  wl_list_init(&pointer_client->resource_list);

  return pointer_client;

err:
  return NULL;
}

static void
zms_pointer_client_destroy(struct zms_pointer_client *pointer_client)
{
  struct wl_resource *resource, *tmp;

  // make the resources inert
  wl_resource_for_each_safe(resource, tmp, &pointer_client->resource_list)
  {
    wl_resource_set_destructor(resource, NULL);
    wl_resource_set_user_data(resource, NULL);
    wl_list_remove(wl_resource_get_link(resource));
  }

  wl_list_remove(&pointer_client->link);
  wl_list_remove(&pointer_client->pointer_destroy_listener.link);
  wl_list_remove(&pointer_client->client_destroy_listener.link);
  free(pointer_client);
}

ZMS_EXPORT struct wl_resource *
zms_pointer_client_resource_create(struct zms_pointer_client *pointer_client,
    struct wl_client *client, uint32_t id)
{
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_pointer_interface, 7, id);
  if (resource == NULL) goto err;

  wl_list_insert(
      &pointer_client->resource_list, wl_resource_get_link(resource));

  wl_resource_set_implementation(resource, &pointer_interface, pointer_client,
      zms_pointer_client_handle_destroy);

  return resource;

err:
  return NULL;
}

ZMS_EXPORT struct wl_resource *
zms_pointer_client_inert_resource_create(struct wl_client *client, uint32_t id)
{
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_pointer_interface, 7, id);
  if (resource == NULL) goto err;

  wl_resource_set_implementation(resource, &pointer_interface, NULL, NULL);

  return resource;

err:
  return NULL;
}

ZMS_EXPORT
struct zms_pointer_client *
zms_pointer_client_find(struct wl_client *client, struct zms_pointer *pointer)
{
  struct zms_pointer_client *pointer_client;

  wl_list_for_each(pointer_client, &pointer->point_client_list, link)
  {
    if (pointer_client->client == client) return pointer_client;
  }

  return NULL;
}

ZMS_EXPORT struct zms_pointer_client *
zms_pointer_client_ensure(struct wl_client *client, struct zms_pointer *pointer)
{
  struct zms_pointer_client *pointer_client;

  pointer_client = zms_pointer_client_find(client, pointer);

  if (pointer_client)
    return pointer_client;
  else
    return zms_pointer_client_create(client, pointer);
}

ZMS_EXPORT void
zms_pointer_client_send_leave(
    struct zms_pointer_client *pointer_client, struct zms_surface *surface)
{
  struct wl_resource *resource;
  struct wl_display *display =
      pointer_client->pointer->seat->priv->compositor->display;
  uint32_t serial = wl_display_next_serial(display);

  wl_resource_for_each(resource, &pointer_client->resource_list)
      wl_pointer_send_leave(resource, serial, surface->resource);
}

ZMS_EXPORT void
zms_pointer_client_send_enter(struct zms_pointer_client *pointer_client,
    uint32_t serial, struct zms_surface *surface, float vx, float vy)
{
  struct wl_resource *resource;

  wl_resource_for_each(resource, &pointer_client->resource_list)
      wl_pointer_send_enter(resource, serial, surface->resource,
          wl_fixed_from_double(vx), wl_fixed_from_double(vy));
}

ZMS_EXPORT void
zms_pointer_client_send_motion(struct zms_pointer_client *pointer_client,
    uint32_t time, float vx, float vy)
{
  struct wl_resource *resource;

  wl_resource_for_each(resource, &pointer_client->resource_list)
      wl_pointer_send_motion(
          resource, time, wl_fixed_from_double(vx), wl_fixed_from_double(vy));
}

ZMS_EXPORT void
zms_pointer_client_send_button(struct zms_pointer_client *pointer_client,
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  struct wl_resource *resource;

  wl_resource_for_each(resource, &pointer_client->resource_list)
      wl_pointer_send_button(resource, serial, time, button, state);
}
