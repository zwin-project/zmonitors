#include "surface.h"

#include <zmonitors.h>

static void zms_surface_destroy(struct zms_surface *surface);

static void
zms_surface_handle_destroy(struct wl_resource *resource)
{
  struct zms_surface *surface;

  surface = wl_resource_get_user_data(resource);

  zms_surface_destroy(surface);
}

static void
zms_surface_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

// TODO:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void
zms_surface_protocol_attach(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *buffer, int32_t x,
    int32_t y)
{}

static void
zms_surface_protocol_damage(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
    int32_t height)
{}

static void
zms_surface_protocol_frame(
    struct wl_client *client, struct wl_resource *resource, uint32_t callback)
{}

static void
zms_surface_protocol_set_opaque_region(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *region)
{}

static void
zms_surface_protocol_set_input_region(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *region)
{}

static void
zms_surface_protocol_commit(
    struct wl_client *client, struct wl_resource *resource)
{}

static void
zms_surface_protocol_set_buffer_transform(
    struct wl_client *client, struct wl_resource *resource, int32_t transform)
{}

static void
zms_surface_protocol_set_buffer_scale(
    struct wl_client *client, struct wl_resource *resource, int32_t scale)
{}

static void
zms_surface_protocol_damage_buffer(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
    int32_t height)
{}
#pragma GCC diagnostic pop

static const struct wl_surface_interface surface_interface = {
    .destroy = zms_surface_protocol_destroy,
    .attach = zms_surface_protocol_attach,
    .damage = zms_surface_protocol_damage,
    .frame = zms_surface_protocol_frame,
    .set_opaque_region = zms_surface_protocol_set_opaque_region,
    .set_input_region = zms_surface_protocol_set_input_region,
    .commit = zms_surface_protocol_commit,
    .set_buffer_transform = zms_surface_protocol_set_buffer_transform,
    .set_buffer_scale = zms_surface_protocol_set_buffer_scale,
    .damage_buffer = zms_surface_protocol_damage_buffer,
};

ZMS_EXPORT struct zms_surface *
zms_surface_create(
    struct zms_compositor *compositor, struct wl_client *client, uint32_t id)
{
  struct zms_surface *surface;
  struct wl_resource *resource;

  surface = zalloc(sizeof *surface);
  if (surface == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &wl_surface_interface, 5, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(
      resource, &surface_interface, surface, zms_surface_handle_destroy);

  surface->resource = resource;
  surface->compositor = compositor;

err_resource:
  free(surface);

err:
  return NULL;
}

static void
zms_surface_destroy(struct zms_surface *surface)
{
  free(surface);
}
