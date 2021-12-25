#ifndef ZMONITORS_SERVER_XDG_SURFACE_H
#define ZMONITORS_SERVER_XDG_SURFACE_H

#include <zmonitors.h>

#include "surface.h"

struct zms_xdg_surface {
  struct zms_surface *surface;
};

struct zms_xdg_surface *zms_xdg_surface_create(
    struct wl_client *client, uint32_t id, struct zms_surface *surface);

#endif  //  ZMONITORS_SERVER_XDG_SURFACE_H
