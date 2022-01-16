#include "pointer.h"

#include <pixman-1/pixman.h>
#include <zmonitors-util.h>

#include "cursor-sprite.h"
#include "output.h"
#include "pointer-client.h"
#include "seat.h"
#include "view.h"

static void
zms_pointer_send_motion(struct zms_pointer* pointer, uint32_t time)
{
  struct zms_view* view = pointer->focus_view_ref.data;
  struct wl_client* client;
  struct zms_pointer_client* pointer_client;

  if (view == NULL) return;

  client = wl_resource_get_client(view->priv->surface->resource);
  pointer_client = zms_pointer_client_find(client, pointer);

  if (pointer_client == NULL) return;

  zms_pointer_client_send_motion(
      pointer_client, time, pointer->vx, pointer->vy);
}

static void
default_grab_focus(struct zms_pointer_grab* grab)
{
  struct zms_pointer* pointer = grab->pointer;
  struct zms_view* view = NULL;
  float vx = 0, vy = 0;

  if (pointer->button_count > 0) return;

  if (pointer->output)
    view =
        zms_output_pick_view(pointer->output, pointer->x, pointer->y, &vx, &vy);

  zms_pointer_set_focus(pointer, view, vx, vy);
}

static void
default_grab_motion_abs(struct zms_pointer_grab* grab,
    struct zms_output* output, uint32_t time, vec2 pos)
{
  struct zms_pointer* pointer = grab->pointer;
  struct zms_view* focus = pointer->focus_view_ref.data;

  if (focus)
    zms_view_get_local_coord(focus, pos[0], pos[1], &pointer->vx, &pointer->vy);

  zms_pointer_move_to(pointer, output, pos[0], pos[1]);
  zms_pointer_send_motion(pointer, time);
}

static void
default_grab_button(struct zms_pointer_grab* grab, uint32_t time,
    uint32_t button, uint32_t state, uint32_t serial)
{
  struct zms_pointer* pointer = grab->pointer;
  struct zms_view* view = pointer->focus_view_ref.data;
  struct wl_client* client;
  struct zms_pointer_client* pointer_client;

  if (view == NULL) return;

  client = wl_resource_get_client(view->priv->surface->resource);
  pointer_client = zms_pointer_client_find(client, pointer);

  if (pointer_client == NULL) return;

  zms_pointer_client_send_button(pointer_client, serial, time, button, state);

  if (pointer->button_count == 0 && state == WL_POINTER_BUTTON_STATE_RELEASED) {
    pointer->grab->interface->focus(pointer->grab);
  }
}

static void
default_grab_cancel(struct zms_pointer_grab* grab)
{
  Z_UNUSED(grab);
}

static const struct zms_pointer_grab_interface default_grab_interface = {
    .focus = default_grab_focus,
    .motion_abs = default_grab_motion_abs,
    .button = default_grab_button,
    .cancel = default_grab_cancel,
};

ZMS_EXPORT struct zms_pointer*
zms_pointer_create(struct zms_seat* seat)
{
  struct zms_pointer* pointer;

  pointer = zalloc(sizeof *pointer);
  if (pointer == NULL) goto err;

  pointer->seat = seat;
  pointer->grab = &pointer->default_grab;
  pointer->default_grab.interface = &default_grab_interface;
  pointer->default_grab.pointer = pointer;
  wl_list_init(&pointer->point_client_list);
  zms_weak_ref_init(&pointer->focus_view_ref);
  zms_weak_ref_init(&pointer->sprite_ref);
  zms_signal_init(&pointer->destroy_signal);
  zms_signal_init(&pointer->moved_signal);

  return pointer;

err:
  return NULL;
}

ZMS_EXPORT void
zms_pointer_destroy(struct zms_pointer* pointer)
{
  pointer->grab->interface->cancel(pointer->grab);
  zms_signal_emit(&pointer->destroy_signal, NULL);
  zms_weak_reference(&pointer->focus_view_ref, NULL, NULL);
  zms_weak_reference(&pointer->sprite_ref, NULL, NULL);
  free(pointer);
}

ZMS_EXPORT void
zms_pointer_set_focus(struct zms_pointer* pointer,
    struct zms_view* view /* nullable */, float vx, float vy)
{
  struct zms_view* current_focus = pointer->focus_view_ref.data;
  struct wl_client* client;
  struct wl_display* display = pointer->seat->priv->compositor->display;

  pointer->vx = vx;
  pointer->vy = vy;

  if (current_focus == view) return;

  if (current_focus) {
    struct zms_pointer_client* pointer_client;
    pointer->enter_serial = 0;

    client = wl_resource_get_client(current_focus->priv->surface->resource);
    pointer_client = zms_pointer_client_find(client, pointer);
    zms_pointer_set_cursor(pointer, NULL, 0, 0);
    if (pointer_client) {
      zms_pointer_client_send_leave(
          pointer_client, current_focus->priv->surface);
    }
  }

  if (view) {
    struct zms_pointer_client* pointer_client;
    uint32_t serial = wl_display_next_serial(display);
    pointer->enter_serial = serial;

    client = wl_resource_get_client(view->priv->surface->resource);
    pointer_client = zms_pointer_client_find(client, pointer);
    if (pointer_client) {
      zms_pointer_client_send_enter(
          pointer_client, serial, view->priv->surface, vx, vy);
    }
  }

  zms_weak_reference(
      &pointer->focus_view_ref, view, view ? &view->unmap_signal : NULL);
}

ZMS_EXPORT void
zms_pointer_move_to(
    struct zms_pointer* pointer, struct zms_output* output, float x, float y)
{
  pointer->output = output;
  pointer->x = x;
  pointer->y = y;

  pointer->grab->interface->focus(pointer->grab);

  zms_signal_emit(&pointer->moved_signal, NULL);
}

ZMS_EXPORT bool
zms_pointer_has_no_grab(struct zms_pointer* pointer)
{
  return pointer->grab == &pointer->default_grab;
}

ZMS_EXPORT void
zms_pointer_start_grab(
    struct zms_pointer* pointer, struct zms_pointer_grab* grab)
{
  if (!zms_pointer_has_no_grab(pointer))
    pointer->grab->interface->cancel(pointer->grab);
  pointer->grab = grab;
  grab->pointer = pointer;
  pointer->grab->interface->focus(pointer->grab);
}

ZMS_EXPORT void
zms_pointer_end_grab(struct zms_pointer* pointer)
{
  pointer->grab = &pointer->default_grab;
  pointer->grab->interface->focus(pointer->grab);
}

ZMS_EXPORT void
zms_pointer_set_cursor(struct zms_pointer* pointer,
    struct zms_surface* surface /*nullable*/, int32_t hotspot_x,
    int32_t hotspot_y)
{
  struct zms_cursor_sprite *current_sprite, *sprite;

  current_sprite = pointer->sprite_ref.data;

  if (surface && current_sprite && current_sprite->surface == surface) {
    zms_cursor_sprite_update_hotspot(current_sprite, hotspot_x, hotspot_y);
  } else {
    if (current_sprite) {
      zms_cursor_sprite_destroy(current_sprite);
      zms_weak_reference(&pointer->sprite_ref, NULL, NULL);
    }

    if (surface) {
      sprite = zms_cursor_sprite_create(pointer, surface, hotspot_x, hotspot_y);
      zms_weak_reference(&pointer->sprite_ref, sprite, &sprite->destroy_signal);
    }
  }
}
