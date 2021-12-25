#include "xdg-surface.h"

#include <xdg-shell-server-protocol.h>
#include <zmonitors.h>

static void zms_xdg_surface_destroy(struct zms_xdg_surface *xdg_surface);

static void
zms_xdg_surface_handle_destroy(struct wl_resource *resource)
{
  struct zms_xdg_surface *xdg_surface;

  xdg_surface = wl_resource_get_user_data(resource);

  zms_xdg_surface_destroy(xdg_surface);
}

static void
zms_xdg_surface_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);

  wl_resource_destroy(resource);
}

static void
zms_xdg_surface_protocol_get_toplevel(
    struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
  // TODO:
  zms_log("request not implemented yet: xdg_surface.get_toplevel\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(id);
}

static void
zms_xdg_surface_protocol_get_popup(struct wl_client *client,
    struct wl_resource *resource, uint32_t id, struct wl_resource *parent,
    struct wl_resource *positioner)
{
  // TODO:
  zms_log("request not implemented yet: xdg_surface.get_popup\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(id);
  Z_UNUSED(parent);
  Z_UNUSED(positioner);
}

static void
zms_xdg_surface_protocol_set_window_geometry(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
    int32_t height)
{
  // TODO:
  zms_log("request not implemented yet: xdg_surface.set_window_geometry\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(x);
  Z_UNUSED(y);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static void
zms_xdg_surface_protocol_ack_configure(
    struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
  // TODO:
  zms_log("request not implemented yet: xdg_surface.ack_configure\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(serial);
}

static const struct xdg_surface_interface zms_xdg_surface_interface = {
    .destroy = zms_xdg_surface_protocol_destroy,
    .get_toplevel = zms_xdg_surface_protocol_get_toplevel,
    .get_popup = zms_xdg_surface_protocol_get_popup,
    .set_window_geometry = zms_xdg_surface_protocol_set_window_geometry,
    .ack_configure = zms_xdg_surface_protocol_ack_configure,
};

ZMS_EXPORT struct zms_xdg_surface *
zms_xdg_surface_create(
    struct zms_surface *surface, struct wl_client *client, uint32_t id)
{
  struct zms_xdg_surface *xdg_surface;
  struct wl_resource *resource;

  xdg_surface = zalloc(sizeof *xdg_surface);
  if (xdg_surface == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &xdg_surface_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(resource, &zms_xdg_surface_interface,
      xdg_surface, zms_xdg_surface_handle_destroy);

  surface->role = SURFACE_ROLE_XDG_SURFACE;
  xdg_surface->surface = surface;

  return xdg_surface;

err_resource:
  free(xdg_surface);

err:
  return NULL;
}

static void
zms_xdg_surface_destroy(struct zms_xdg_surface *xdg_surface)
{
  free(xdg_surface);
}
