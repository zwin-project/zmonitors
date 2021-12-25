#ifndef ZMONITORS_SERVER_SURFACE_H
#define ZMONITORS_SERVER_SURFACE_H

#include <wayland-server.h>
#include <zmonitors.h>

#include "compositor.h"
#include "view.h"

enum zms_surface_role {
  SURFACE_ROLE_NONE = 0,
  SURFACE_ROLE_XDG_SURFACE,
  SURFACE_ROLE_XDG_TOPLEVEL,
  SURFACE_ROLE_XDG_POPUP,
};

struct zms_surface {
  struct wl_resource *resource;

  struct zms_compositor *compositor;
  struct zms_view *view;

  enum zms_surface_role role;
};

struct zms_surface *zms_surface_create(
    struct zms_compositor *compositor, struct wl_client *client, uint32_t id);

#endif  //  ZMONITORS_SERVER_SURFACE_H
