#include "compositor.h"

#include <wayland-server.h>
#include <zmonitors.h>

static void
zms_compositor_protocol_create_surface(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  // TODO:
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(id);
}

static void
zms_compositor_protocol_create_region(
    struct wl_client* client, struct wl_resource* resource, uint32_t id)
{
  // TODO:
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(id);
}

static const struct wl_compositor_interface compositor_interface = {
    .create_surface = zms_compositor_protocol_create_surface,
    .create_region = zms_compositor_protocol_create_region,
};

static void
zms_compositor_bind(
    struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
  Z_UNUSED(version);
  struct zms_compositor* compositor = data;
  struct wl_resource* resource;

  resource = wl_resource_create(client, &wl_compositor_interface, 4, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(
      resource, &compositor_interface, compositor, NULL);
}

ZMS_EXPORT struct zms_compositor*
zms_compositor_create()
{
  struct zms_compositor* compositor;
  struct zms_compositor_private* priv;
  struct wl_display* display;
  struct wl_global* global;
  const char* socket;

  display = wl_display_create();
  if (display == NULL) {
    zms_log("failed to create a display\n");
    goto err_display;
  }

  compositor = zalloc(sizeof *compositor);
  if (compositor == NULL) {
    zms_log("failed to create a compositor\n");
    goto err_compositor;
  }

  priv = zalloc(sizeof *priv);
  if (priv == NULL) {
    zms_log("failed to create a compositor priv\n");
    goto err_priv;
  }

  global = wl_global_create(
      display, &wl_compositor_interface, 4, compositor, zms_compositor_bind);
  if (global == NULL) {
    zms_log("failed to create a compositor wl_global\n");
    goto err_global;
  }

  socket = wl_display_add_socket_auto(display);
  if (socket == NULL) {
    zms_log("failed to create a socket\n");
    goto err_socket;
  }

  priv->display = display;
  priv->global = global;

  compositor->priv = priv;

  return compositor;

err_socket:
err_global:
  free(priv);

err_priv:
  free(compositor);

err_compositor:
  wl_display_destroy(display);

err_display:
  return NULL;
}

WL_EXPORT void
zms_compositor_destroy(struct zms_compositor* compositor)
{
  wl_display_destroy(compositor->priv->display);
  free(compositor->priv);
  free(compositor);
}

WL_EXPORT void
zms_compositor_flush_clients(struct zms_compositor* compositor)
{
  wl_display_flush_clients(compositor->priv->display);
}

WL_EXPORT void
zms_compositor_dispatch_event(struct zms_compositor* compositor, int timeout)
{
  wl_event_loop_dispatch(
      wl_display_get_event_loop(compositor->priv->display), timeout);
}
