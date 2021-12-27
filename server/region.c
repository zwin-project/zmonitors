#include "region.h"

#include <wayland-server.h>
#include <zmonitors.h>

static void zms_region_destroy(struct zms_region *region);

static void
zms_region_handle_destroy(struct wl_resource *resource)
{
  struct zms_region *region;

  region = wl_resource_get_user_data(resource);

  zms_region_destroy(region);
}

static void
zms_region_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);

  wl_resource_destroy(resource);
}

static void
zms_region_protocol_add(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
  zms_log("request not implemented yet: wl_region.add\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(x);
  Z_UNUSED(y);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static void
zms_region_protocol_subtract(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
    int32_t height)
{
  zms_log("request not implemented yet: wl_region.subtract\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(x);
  Z_UNUSED(y);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static const struct wl_region_interface region_interface = {
    .destroy = zms_region_protocol_destroy,
    .add = zms_region_protocol_add,
    .subtract = zms_region_protocol_subtract,
};

ZMS_EXPORT struct zms_region *
zms_region_create(struct wl_client *client, uint32_t id)
{
  struct zms_region *region;
  struct wl_resource *resource;

  region = zalloc(sizeof *region);
  if (region == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &wl_region_interface, 1, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(
      resource, &region_interface, region, zms_region_handle_destroy);

  region->resource = resource;

  return region;

err_resource:
  free(region);

err:
  return NULL;
}

static void
zms_region_destroy(struct zms_region *region)
{
  free(region);
}
