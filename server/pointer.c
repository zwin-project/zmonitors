#include "pointer.h"

#include <zmonitors-util.h>

#include "output.h"
#include "pointer-client.h"
#include "view.h"

static void
zms_pointer_move_to(
    struct zms_pointer* pointer, struct zms_output* output, float x, float y)
{
  pointer->output = output;
  pointer->x = x;
  pointer->y = y;
  // TODO: update cursor pos

  pointer->grab->interface->focus(pointer->grab);
}

static void
zms_pointer_set_focus(struct zms_pointer* pointer,
    struct zms_view* view /* nullable */, float vx, float vy)
{
  struct zms_view* current_focus = pointer->focus_view_ref.data;
  struct wl_client* client;

  pointer->vx = vx;
  pointer->vy = vy;

  if (current_focus == view) return;

  if (current_focus) {
    struct zms_pointer_client* pointer_client;
    client = wl_resource_get_client(current_focus->priv->surface->resource);
    pointer_client = zms_pointer_client_find(client, pointer);
    if (pointer_client) {
      zms_pointer_client_send_leave(
          pointer_client, current_focus->priv->surface);
    }
  }

  if (view) {
    struct zms_pointer_client* pointer_client;
    client = wl_resource_get_client(view->priv->surface->resource);
    pointer_client = zms_pointer_client_find(client, pointer);
    if (pointer_client) {
      zms_pointer_client_send_enter(
          pointer_client, view->priv->surface, vx, vy);
    }
  }

  zms_weak_reference(&pointer->focus_view_ref, view, &view->destroy_signal);
}

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
  struct zms_view* view;
  float vx, vy;

  if (!pointer->output || pointer->button_count > 0) return;

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
  struct zms_pointer* pointer = grab->pointer;
  zms_pointer_move_to(pointer, NULL, 0, 0);
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
  zms_signal_init(&pointer->destroy_signal);

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
  free(pointer);
}
