#ifndef ZMONITORS_SERVER_SEAT_H
#define ZMONITORS_SERVER_SEAT_H

#include <wayland-server.h>
#include <zmonitors.h>

#include "compositor.h"

struct zms_seat_private {
  struct wl_global *global;
  struct zms_compositor *compositor;
};

struct zms_seat *zms_seat_create(struct zms_compositor *compositor);

void zms_seat_destroy(struct zms_seat *seat);

#endif  //  ZMONITORS_SERVER_SEAT_H
