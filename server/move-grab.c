#include "move-grab.h"

#include "pixman-helper.h"
#include "pointer.h"
#include "seat.h"
#include "xdg-toplevel.h"

struct zms_move_grab {
  struct zms_pointer_grab base;
  struct zms_view* view; /* nonnull & mapped */

  struct zms_listener view_unmap_listener;

  float dx, dy;
};

static void zms_move_grab_destroy(struct zms_move_grab* grab);

static void
zms_move_grab_focus(struct zms_pointer_grab* grab)
{
  Z_UNUSED(grab);
}

static void
zms_move_grab_motion_abs(struct zms_pointer_grab* grab,
    struct zms_output* output, uint32_t time, vec2 pos)
{
  Z_UNUSED(time);
  struct zms_move_grab* move_grab = wl_container_of(grab, move_grab, base);

  pixman_region32_t damage;
  pixman_region32_t old_view_region;
  pixman_region32_t new_view_region;

  pixman_region32_init(&damage);
  pixman_region32_init_view_global(&old_view_region, move_grab->view);
  zms_view_set_origin(
      move_grab->view, move_grab->dx + pos[0], move_grab->dy + pos[1]);
  pixman_region32_init_view_global(&new_view_region, move_grab->view);

  pixman_region32_union(&damage, &old_view_region, &new_view_region);

  zms_output_render(output, &damage);

  pixman_region32_fini(&damage);
  pixman_region32_fini(&old_view_region);
  pixman_region32_fini(&new_view_region);
}

static void
zms_move_grab_button(struct zms_pointer_grab* grab, uint32_t time,
    uint32_t button, uint32_t state, uint32_t serial)
{
  Z_UNUSED(time);
  Z_UNUSED(button);
  Z_UNUSED(serial);
  if (grab->pointer->button_count == 0 &&
      state == WL_POINTER_BUTTON_STATE_RELEASED) {
    grab->interface->cancel(grab);
  }
}

static void
zms_move_grab_cancel(struct zms_pointer_grab* grab)
{
  struct zms_move_grab* move_grab = wl_container_of(grab, move_grab, base);
  zms_pointer_end_grab(grab->pointer);
  zms_move_grab_destroy(move_grab);
}

static const struct zms_pointer_grab_interface move_grab_interface = {
    .focus = zms_move_grab_focus,
    .motion_abs = zms_move_grab_motion_abs,
    .button = zms_move_grab_button,
    .cancel = zms_move_grab_cancel,
};

static void
view_destroy_handler(struct zms_listener* listener, void* data)
{
  Z_UNUSED(data);
  struct zms_move_grab* grab;

  grab = wl_container_of(listener, grab, view_unmap_listener);

  grab->base.interface->cancel(&grab->base);
}

static struct zms_move_grab*
zms_move_grab_create(struct zms_view* view)
{
  struct zms_move_grab* grab;

  grab = zalloc(sizeof *grab);
  if (grab == NULL) goto err;

  grab->base.interface = &move_grab_interface;
  grab->view = view;
  grab->view_unmap_listener.notify = view_destroy_handler;
  zms_signal_add(&view->unmap_signal, &grab->view_unmap_listener);

  return grab;

err:
  return NULL;
}

static void
zms_move_grab_destroy(struct zms_move_grab* grab)
{
  wl_list_remove(&grab->view_unmap_listener.link);
  free(grab);
}

ZMS_EXPORT bool
zms_move_grab_start(
    struct zms_seat* seat, struct zms_view* view, uint32_t serial)
{
  struct zms_move_grab* grab;
  struct zms_pointer* pointer;

  if (zms_view_is_mapped(view) == false) return false;

  pointer = seat->priv->pointer;
  if (pointer == NULL) return false;

  grab = zms_move_grab_create(view);
  if (grab == NULL) return false;

  if (pointer->button_count > 0 && pointer->grab_serial == serial &&
      pointer->focus_view_ref.data == view &&
      zms_pointer_has_no_grab(pointer)) {
    grab->dx = view->priv->origin[0] - pointer->grab_x;
    grab->dy = view->priv->origin[1] - pointer->grab_y;
    zms_pointer_start_grab(pointer, &grab->base);
    zms_pointer_set_focus(pointer, NULL, 0, 0);
    return true;
  }

  zms_move_grab_destroy(grab);

  return false;
}
