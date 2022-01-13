#ifndef ZMONITORS_SERVER_POINTER_H
#define ZMONITORS_SERVER_POINTER_H

#include <wayland-server.h>
#include <zmonitors-util.h>

#include "output.h"

struct zms_pointer_grab {
  const struct zms_pointer_grab_interface* interface;
  struct zms_pointer* pointer;
};

struct zms_pointer_grab_interface {
  void (*focus)(struct zms_pointer_grab* grab);
  void (*motion_abs)(struct zms_pointer_grab* grab, struct zms_output* output,
      uint32_t time, vec2 pos);
  void (*button)(struct zms_pointer_grab* grab, uint32_t time, uint32_t button,
      uint32_t state, uint32_t serial);
  void (*cancel)(struct zms_pointer_grab* grab);
};

struct zms_pointer {
  struct zms_seat* seat;
  struct zms_pointer_grab* grab;
  struct zms_pointer_grab default_grab;
  uint32_t button_count;

  struct zms_output* output; /* nullable */
  float x, y;

  struct wl_list point_client_list;

  struct zms_weak_ref focus_view_ref;
  float vx, vy;

  // signals
  struct zms_signal destroy_signal;
};

struct zms_pointer* zms_pointer_create(struct zms_seat* seat);

void zms_pointer_destroy(struct zms_pointer* pointer);

#endif  //  ZMONITORS_SERVER_POINTER_H
