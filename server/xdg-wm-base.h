#ifndef ZMONITORS_SERVER_XDG_WM_BASE_H
#define ZMONITORS_SERVER_XDG_WM_BASE_H

#include <wayland-server.h>

#include "compositor.h"

struct zms_wm_base {
  struct wl_global *global;
  struct zms_compositor *compositor;
};

struct zms_wm_base *zms_wm_base_create(struct zms_compositor *comopsitor);

void zms_wm_base_destroy(struct zms_wm_base *wm_base);

#endif  //  ZMONITORS_SERVER_XDG_WM_BASE_H
