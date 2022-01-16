#ifndef ZMONITORS_SERVER_POINTER_H
#define ZMONITORS_SERVER_POINTER_H

#include <wayland-server.h>
#include <zmonitors-util.h>

#include "output.h"
#include "surface.h"

struct zms_pointer_grab {
  const struct zms_pointer_grab_interface* interface;
  struct zms_pointer* pointer; /* nonnull */
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
  uint32_t grab_serial;

  float grab_x, grab_y;

  struct zms_output* output; /* nullable */
  float x, y;

  struct wl_list point_client_list;

  struct zms_weak_ref focus_view_ref; /** keeps *mapped* view */
  float vx, vy;
  uint32_t enter_serial;

  struct zms_weak_ref sprite_ref;

  // signals
  struct zms_signal destroy_signal;
  struct zms_signal moved_signal;
};

struct zms_pointer* zms_pointer_create(struct zms_seat* seat);

void zms_pointer_destroy(struct zms_pointer* pointer);

void zms_pointer_set_focus(struct zms_pointer* pointer,
    struct zms_view* view /* nullable */, float vx, float vy);

void zms_pointer_move_to(
    struct zms_pointer* pointer, struct zms_output* output, float x, float y);

bool zms_pointer_has_no_grab(struct zms_pointer* pointer);

void zms_pointer_start_grab(
    struct zms_pointer* pointer, struct zms_pointer_grab* grab);

void zms_pointer_end_grab(struct zms_pointer* pointer);

void zms_pointer_set_cursor(struct zms_pointer* pointer,
    struct zms_surface* surface /*nullable*/, int32_t hotspot_x,
    int32_t hotspot_y);

#endif  //  ZMONITORS_SERVER_POINTER_H
