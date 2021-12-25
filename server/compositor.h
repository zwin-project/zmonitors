#ifndef ZMONITORS_SERVER_COMPOSITOR_H
#define ZMONITORS_SERVER_COMPOSITOR_H

#include <wayland-server.h>

struct zms_compositor_private {
  struct wl_display *display;
  struct wl_global *global;
};

#endif  //  ZMONITORS_SERVER_COMPOSITOR_H
