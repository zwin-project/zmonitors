#include "data-device.h"

#include <zmonitors-util.h>

static void
zms_data_device_handle_destroy(struct wl_resource* resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void
zms_data_device_protocol_start_drag(struct wl_client* client,
    struct wl_resource* resource, struct wl_resource* source,
    struct wl_resource* origin, struct wl_resource* icon, uint32_t serial)
{
  // TODO:
  zms_log("request not implemented yet: wl_data_device.start_drag\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(source);
  Z_UNUSED(origin);
  Z_UNUSED(icon);
  Z_UNUSED(serial);
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

ZMS_EXPORT struct zms_data_device*
zms_data_device_create()
{
  struct zms_data_device* data_device;

  data_device = zalloc(sizeof *data_device);
  if (data_device == NULL) goto err;

  wl_list_init(&data_device->resource_list);

  return data_device;

err:
  return NULL;
}

ZMS_EXPORT void
zms_data_device_destroy(struct zms_data_device* data_device)
{
  struct wl_resource *resource, *tmp;

  wl_resource_for_each_safe(resource, tmp, &data_device->resource_list)
      wl_resource_destroy(resource);

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
