#ifndef ZMONITORS_SERVER_XDG_SURFACE_H
#define ZMONITORS_SERVER_XDG_SURFACE_H

#include <wayland-server.h>
#include <zmonitors.h>

#include "surface.h"

struct zms_xdg_surface {
  struct wl_resource *resource;

  struct zms_surface *surface; /* nonnull */

  /* listeners */
  struct wl_listener surface_destroy_listener;

  /* signals */
  struct wl_signal destroy_signal;
};

struct zms_xdg_surface *zms_xdg_surface_create(
    struct wl_client *client, uint32_t id, struct zms_surface *surface);

#endif  //  ZMONITORS_SERVER_XDG_SURFACE_H
