#ifndef ZMONITORS_SERVER_XDG_TOPLEVEL_H
#define ZMONITORS_SERVER_XDG_TOPLEVEL_H

#include <wayland-server.h>

#include "xdg-surface.h"

struct zms_xdg_toplevel {
  struct wl_resource *resource;
  struct zms_xdg_surface *xdg_surface; /* nonnull */
  struct wl_listener xdg_surface_destroy_listener;
};

struct zms_xdg_toplevel *zms_xdg_toplevel_create(
    struct wl_client *client, uint32_t id, struct zms_xdg_surface *xdg_surface);

#endif  //  ZMONITORS_SERVER_XDG_TOPLEVEL_H
