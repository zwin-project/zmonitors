#include "pointer.h"

#include <zmonitors-util.h>

static void
zms_pointer_handle_destroy(struct wl_resource* resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void
zms_pointer_protocol_set_cursor(struct wl_client* client,
    struct wl_resource* resource, uint32_t serial,
    struct wl_resource* surface_resource, int32_t hotspot_x, int32_t hotspot_y)
{
  zms_log("request not implemented yet: wl_pointer.set_cursor\n");
  Z_UNUSED(client);
  Z_UNUSED(serial);
  Z_UNUSED(surface_resource);
  Z_UNUSED(hotspot_x);
  Z_UNUSED(hotspot_y);
  struct zms_pointer* pointer = wl_resource_get_user_data(resource);
  if (pointer == NULL) return; /* inert resource */
  // TODO:
}

static void
zms_pointer_protocol_release(
    struct wl_client* client, struct wl_resource* resource)
{
  Z_UNUSED(client);

  wl_resource_destroy(resource);
}

static const struct wl_pointer_interface pointer_interface = {
    .set_cursor = zms_pointer_protocol_set_cursor,
    .release = zms_pointer_protocol_release,
};

ZMS_EXPORT struct zms_pointer*
zms_pointer_create()
{
  struct zms_pointer* pointer;

  pointer = zalloc(sizeof *pointer);
  if (pointer == NULL) goto err;

  wl_list_init(&pointer->resource_list);

  return pointer;

err:
  return NULL;
}

ZMS_EXPORT void
zms_pointer_destroy(struct zms_pointer* pointer)
{
  struct wl_resource *resource, *tmp;

  // make the resources inert
  wl_resource_for_each_safe(resource, tmp, &pointer->resource_list)
  {
    wl_resource_set_destructor(resource, NULL);
    wl_resource_set_user_data(resource, NULL);
    wl_list_remove(wl_resource_get_link(resource));
  }

  free(pointer);
}

ZMS_EXPORT struct wl_resource*
zms_pointer_resource_create(
    struct zms_pointer* pointer, struct wl_client* client, uint32_t id)
{
  struct wl_resource* resource;

  resource = wl_resource_create(client, &wl_pointer_interface, 7, id);
  if (resource == NULL) goto err;

  wl_list_insert(&pointer->resource_list, wl_resource_get_link(resource));

  wl_resource_set_implementation(
      resource, &pointer_interface, pointer, zms_pointer_handle_destroy);

  return resource;

err:
  return NULL;
}

ZMS_EXPORT struct wl_resource*
zms_pointer_inert_resource_create(struct wl_client* client, uint32_t id)
{
  struct wl_resource* resource;

  resource = wl_resource_create(client, &wl_pointer_interface, 7, id);
  if (resource == NULL) goto err;

  wl_resource_set_implementation(resource, &pointer_interface, NULL, NULL);

  return resource;

err:
  return NULL;
}
