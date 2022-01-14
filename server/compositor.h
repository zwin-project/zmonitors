#ifndef ZMONITORS_SERVER_COMPOSITOR_H
#define ZMONITORS_SERVER_COMPOSITOR_H

#include <wayland-server.h>
#include <zmonitors-server.h>

#include "xdg-wm-base.h"

struct zms_compositor_private {
  /* global objects */
  struct zms_wm_base* wm_base;

  struct wl_list output_list;
};

struct zms_output* zms_compositor_get_primary_output(
    struct zms_compositor* compositor);

#endif  //  ZMONITORS_SERVER_COMPOSITOR_H
