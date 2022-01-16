#include "xdg-wm-base.h"

#include <xdg-shell-server-protocol.h>
#include <zmonitors-server.h>

#include "surface.h"
#include "xdg-positioner.h"
#include "xdg-surface.h"

static void
zms_wm_base_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static void
zms_wm_base_protocol_create_positioner(
    struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
  Z_UNUSED(resource);
  zms_xdg_positioner_create(client, id);
}

static void
zms_wm_base_protocol_get_xdg_surface(struct wl_client *client,
    struct wl_resource *resource, uint32_t id,
    struct wl_resource *surface_resource)
{
  Z_UNUSED(resource);
  struct zms_surface *surface;

  surface = wl_resource_get_user_data(surface_resource);

  zms_xdg_surface_create(client, id, surface);
}

static void
zms_wm_base_protocol_pong(
    struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
  // TODO:
  zms_log("request not implemented yet: xdg_wm_base.pong\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(serial);
}

static const struct xdg_wm_base_interface wm_base_interface = {
    .destroy = zms_wm_base_protocol_destroy,
    .create_positioner = zms_wm_base_protocol_create_positioner,
    .get_xdg_surface = zms_wm_base_protocol_get_xdg_surface,
    .pong = zms_wm_base_protocol_pong,
};

static void
zms_wm_base_bind(
    struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
  struct zms_wm_base *wm_base = data;
  struct wl_resource *resource;

  resource = wl_resource_create(client, &xdg_wm_base_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(resource, &wm_base_interface, wm_base, NULL);
}

ZMS_EXPORT struct zms_wm_base *
zms_wm_base_create(struct zms_compositor *comopsitor)
{
  struct zms_wm_base *wm_base;
  struct wl_global *global;

  wm_base = zalloc(sizeof *wm_base);
  if (wm_base == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  global = wl_global_create(comopsitor->display, &xdg_wm_base_interface, 3,
      wm_base, zms_wm_base_bind);
  if (global == NULL) {
    zms_log("failed to create a wm_base wl_global\n");
    goto err_global;
  }

  wm_base->global = global;
  wm_base->compositor = comopsitor;

  return wm_base;

err_global:
  free(wm_base);

err:
  return NULL;
}

ZMS_EXPORT void
zms_wm_base_destroy(struct zms_wm_base *wm_base)
{
  wl_global_destroy(wm_base->global);
  free(wm_base);
}
