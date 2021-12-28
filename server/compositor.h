#ifndef ZMONITORS_SERVER_COMPOSITOR_H
#define ZMONITORS_SERVER_COMPOSITOR_H

#include <wayland-server.h>

#include "xdg-wm-base.h"

struct zms_compositor_private {
  /* global objects */
  struct zms_wm_base *wm_base;
};

#endif  //  ZMONITORS_SERVER_COMPOSITOR_H
