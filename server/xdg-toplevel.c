#include "xdg-toplevel.h"

#include <xdg-shell-server-protocol.h>
#include <zmonitors-server.h>

#include "move-grab.h"
#include "output.h"

static void zms_xdg_toplevel_destroy(struct zms_xdg_toplevel *toplevel);

static void
zms_xdg_toplevel_handle_destroy(struct wl_resource *resource)
{
  struct zms_xdg_toplevel *toplevel;

  toplevel = wl_resource_get_user_data(resource);

  zms_xdg_toplevel_destroy(toplevel);
}

static void
zms_xdg_toplevel_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);

  wl_resource_destroy(resource);
}

static void
zms_xdg_toplevel_protocol_set_parent(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *parent)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_parent\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(parent);
}

static void
zms_xdg_toplevel_protocol_set_title(
    struct wl_client *client, struct wl_resource *resource, const char *title)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_title\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(title);
}

static void
zms_xdg_toplevel_protocol_set_app_id(
    struct wl_client *client, struct wl_resource *resource, const char *app_id)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_app_id\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(app_id);
}

static void
zms_xdg_toplevel_protocol_show_window_menu(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *seat, uint32_t serial,
    int32_t x, int32_t y)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.show_window_menu\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(seat);
  Z_UNUSED(serial);
  Z_UNUSED(x);
  Z_UNUSED(y);
}

static void
zms_xdg_toplevel_protocol_move(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *seat_resource,
    uint32_t serial)
{
  Z_UNUSED(client);
  struct zms_xdg_toplevel *toplevel;
  struct zms_seat *seat;

  seat = wl_resource_get_user_data(seat_resource);
  toplevel = wl_resource_get_user_data(resource);

  zms_move_grab_start(seat, toplevel->xdg_surface->surface->view, serial);
}

static void
zms_xdg_toplevel_protocol_resize(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *seat, uint32_t serial,
    uint32_t edges)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.resize\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(seat);
  Z_UNUSED(serial);
  Z_UNUSED(edges);
}

static void
zms_xdg_toplevel_protocol_set_max_size(struct wl_client *client,
    struct wl_resource *resource, int32_t width, int32_t height)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_max_size\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static void
zms_xdg_toplevel_protocol_set_min_size(struct wl_client *client,
    struct wl_resource *resource, int32_t width, int32_t height)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_min_size\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static void
zms_xdg_toplevel_protocol_set_maximized(
    struct wl_client *client, struct wl_resource *resource)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_maximized\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
}

static void
zms_xdg_toplevel_protocol_unset_maximized(
    struct wl_client *client, struct wl_resource *resource)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.unset_maximized\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
}

static void
zms_xdg_toplevel_protocol_set_fullscreen(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *output)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_fullscreen\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(output);
}

static void
zms_xdg_toplevel_protocol_unset_fullscreen(
    struct wl_client *client, struct wl_resource *resource)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.unset_fullscreen\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
}

static void
zms_xdg_toplevel_protocol_set_minimized(
    struct wl_client *client, struct wl_resource *resource)
{
  // TODO:
  zms_log("request not implemented yet: xdg_toplevel.set_minimized\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
}

static const struct xdg_toplevel_interface toplevel_interface = {
    .destroy = zms_xdg_toplevel_protocol_destroy,
    .set_parent = zms_xdg_toplevel_protocol_set_parent,
    .set_title = zms_xdg_toplevel_protocol_set_title,
    .set_app_id = zms_xdg_toplevel_protocol_set_app_id,
    .show_window_menu = zms_xdg_toplevel_protocol_show_window_menu,
    .move = zms_xdg_toplevel_protocol_move,
    .resize = zms_xdg_toplevel_protocol_resize,
    .set_max_size = zms_xdg_toplevel_protocol_set_max_size,
    .set_min_size = zms_xdg_toplevel_protocol_set_min_size,
    .set_maximized = zms_xdg_toplevel_protocol_set_maximized,
    .unset_maximized = zms_xdg_toplevel_protocol_unset_maximized,
    .set_fullscreen = zms_xdg_toplevel_protocol_set_fullscreen,
    .unset_fullscreen = zms_xdg_toplevel_protocol_unset_fullscreen,
    .set_minimized = zms_xdg_toplevel_protocol_set_minimized,
};

static void
zms_xdg_toplevel_send_configure(struct zms_xdg_toplevel *toplevel)
{
  struct zms_xdg_toplevel_configuration *config;
  struct wl_display *display =
      toplevel->xdg_surface->surface->compositor->display;
  struct wl_array state;

  config = zalloc(sizeof *config);
  if (config == NULL) {
    wl_client_post_no_memory(wl_resource_get_client(toplevel->resource));
    return;
  }

  config->serial = wl_display_next_serial(display);
  config->size = toplevel->pending.size;

  wl_list_insert(&toplevel->config_list, &config->link);

  wl_array_init(&state);
  // TODO: set state

  xdg_toplevel_send_configure(
      toplevel->resource, config->size.width, config->size.height, &state);

  wl_array_release(&state);

  xdg_surface_send_configure(toplevel->xdg_surface->resource, config->serial);
}

static void
surface_commit_signal_handler(struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_xdg_toplevel *toplevel;
  struct zms_surface *surface;
  struct zms_view *view;

  toplevel = wl_container_of(listener, toplevel, surface_commit_listener);
  surface = toplevel->xdg_surface->surface;
  view = surface->view;

  if (!toplevel->committed) {
    toplevel->committed = true;
    // TODO: check that the surface has no attached buffer
    zms_xdg_toplevel_send_configure(toplevel);
    return;
  }

  if (zms_view_has_image(view) && zms_view_is_mapped(view) == false) {
    struct zms_output *primary_output =
        zms_compositor_get_primary_output(surface->compositor);
    zms_view_set_origin(view,
        (primary_output->priv->size.width - zms_view_get_width(view)) / 2,
        (primary_output->priv->size.height - zms_view_get_height(view)) / 2);
    zms_output_map_view(
        primary_output, surface->view, ZMS_OUTPUT_MAIN_LAYER_INDEX);
  }
}

static void
xdg_surface_destroy_signal_handler(struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);

  struct zms_xdg_toplevel *toplevel;

  toplevel = wl_container_of(listener, toplevel, xdg_surface_destroy_listener);

  wl_resource_destroy(toplevel->resource);
}

ZMS_EXPORT struct zms_xdg_toplevel *
zms_xdg_toplevel_create(
    struct wl_client *client, uint32_t id, struct zms_xdg_surface *xdg_surface)
{
  struct zms_xdg_toplevel *toplevel;
  struct wl_resource *resource;

  toplevel = zalloc(sizeof *toplevel);
  if (toplevel == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &xdg_toplevel_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(
      resource, &toplevel_interface, toplevel, zms_xdg_toplevel_handle_destroy);

  toplevel->resource = resource;
  toplevel->xdg_surface = xdg_surface;
  if (zms_surface_set_role(xdg_surface->surface, SURFACE_ROLE_XDG_TOPLEVEL,
          xdg_surface->resource, XDG_WM_BASE_ERROR_ROLE) < 0)
    goto err_role;
  xdg_surface->surface->role_object = toplevel;

  wl_list_init(&toplevel->config_list);

  // toplevel->pending was initalized by zalloc

  toplevel->surface_commit_listener.notify = surface_commit_signal_handler;
  zms_signal_add(
      &xdg_surface->surface->commit_signal, &toplevel->surface_commit_listener);

  toplevel->xdg_surface_destroy_listener.notify =
      xdg_surface_destroy_signal_handler;
  zms_signal_add(
      &xdg_surface->destroy_signal, &toplevel->xdg_surface_destroy_listener);

  toplevel->committed = false;

  return toplevel;

err_role:
err_resource:
  free(toplevel);

err:
  return NULL;
}

static void
zms_xdg_toplevel_destroy(struct zms_xdg_toplevel *toplevel)
{
  struct zms_xdg_toplevel_configuration *config, *tmp;

  wl_list_for_each_safe(config, tmp, &toplevel->config_list, link)
  {
    wl_list_remove(&config->link);
    free(config);
  }

  toplevel->xdg_surface->surface->role_object = NULL;
  wl_list_remove(&toplevel->surface_commit_listener.link);
  wl_list_remove(&toplevel->xdg_surface_destroy_listener.link);
  free(toplevel);
}
