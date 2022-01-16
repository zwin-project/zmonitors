#ifndef ZMONITORS_SERVER_XDG_POSITIONER_H
#define ZMONITORS_SERVER_XDG_POSITIONER_H

#include <wayland-server.h>

struct zms_xdg_positioner {
  struct wl_resource *resource;
};

struct zms_xdg_positioner *zms_xdg_positioner_create(
    struct wl_client *client, uint32_t id);

#endif  //  ZMONITORS_SERVER_XDG_POSITIONER_H
