#ifndef ZMONITORS_SERVER_CURSOR_SPRITE_H
#define ZMONITORS_SERVER_CURSOR_SPRITE_H

#include "pointer.h"
#include "view.h"

struct zms_cursor_sprite {
  struct zms_pointer
      *pointer; /* nonnull; self->pointer->sprite_ref.data must be self */
  struct zms_surface *surface; /* nonnull */

  // signals
  struct zms_signal destroy_signal;

  // listeners
  struct zms_listener surface_destroy_listener;
  struct zms_listener surface_commit_listener;
  struct zms_listener pointer_destroy_listener;
  struct zms_listener pointer_moved_listener;

  struct {
    int32_t x, y;
  } hotspot;
};

struct zms_cursor_sprite *zms_cursor_sprite_create(struct zms_pointer *pointer,
    struct zms_surface *surface, int32_t hotspot_x, int32_t hotspot_y);

void zms_cursor_sprite_destroy(struct zms_cursor_sprite *cursor_sprite);

void zms_cursor_sprite_update_hotspot(
    struct zms_cursor_sprite *sprite, int32_t hotspot_x, int32_t hotspot_y);

#endif  //  ZMONITORS_SERVER_CURSOR_SPRITE_H
