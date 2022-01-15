#ifndef ZMONITORS_SERVER_XDG_TOPLEVEL_H
#define ZMONITORS_SERVER_XDG_TOPLEVEL_H

#include <wayland-server.h>
#include <zmonitors-types.h>

#include "xdg-surface.h"

struct zms_xdg_toplevel_configuration {
  uint32_t serial;
  struct wl_list link;  // -> zms_xdg_toplevel.config_list

  struct zms_screen_size size;
};

struct zms_xdg_toplevel {
  struct wl_resource *resource;
  struct zms_xdg_surface *xdg_surface; /* nonnull */

  struct wl_list config_list;

  struct {
    struct zms_screen_size size;
  } pending;

  /* listeners */
  struct zms_listener surface_commit_listener;
  struct zms_listener xdg_surface_destroy_listener;

  bool committed;
};

struct zms_xdg_toplevel *zms_xdg_toplevel_create(
    struct wl_client *client, uint32_t id, struct zms_xdg_surface *xdg_surface);

#endif  //  ZMONITORS_SERVER_XDG_TOPLEVEL_H
