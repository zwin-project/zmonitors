#ifndef ZMONITORS_SERVER_COMPOSITOR_H
#define ZMONITORS_SERVER_COMPOSITOR_H

#include <wayland-server.h>

#include "xdg_wm_base.h"

struct zms_compositor_private {
  struct wl_display *display;

  /* global objects */
  struct zms_wm_base *wm_base;
};

#endif  //  ZMONITORS_SERVER_COMPOSITOR_H
