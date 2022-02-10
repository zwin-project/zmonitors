#include "drag-grab.h"

#include <zmonitors-util.h>

#include "seat.h"

struct zms_drag_grab {
  struct zms_pointer_grab base;
};

static void zms_drag_grab_destroy(struct zms_drag_grab *drag_grab);

static void
zms_drag_grab_focus(struct zms_pointer_grab *grab)
{
  struct zms_pointer *pointer = grab->pointer;
  struct zms_view *view = NULL;
  float vx = 0, vy = 0;

  if (pointer->output)
    view =
        zms_output_pick_view(pointer->output, pointer->x, pointer->y, &vx, &vy);

  zms_data_device_set_focus(pointer->seat->data_device, view, vx, vy);
}

static void
zms_drag_grab_motion_abs(struct zms_pointer_grab *grab,
    struct zms_output *output, uint32_t time, vec2 pos)
{
  struct zms_pointer *pointer = grab->pointer;

  zms_pointer_move_to(pointer, output, pos[0], pos[1]);
  zms_data_device_send_motion(pointer->seat->data_device, time);
}

static void
zms_drag_grab_button(struct zms_pointer_grab *grab, uint32_t time,
    uint32_t button, uint32_t state, uint32_t serial)
{
  Z_UNUSED(grab);
  Z_UNUSED(time);
  Z_UNUSED(button);
  Z_UNUSED(state);
  Z_UNUSED(serial);
}

static void
zms_drag_grab_axis(
    struct zms_pointer_grab *grab, uint32_t time, uint32_t axis, float value)
{
  Z_UNUSED(grab);
  Z_UNUSED(time);
  Z_UNUSED(axis);
  Z_UNUSED(value);
}

static void
zms_drag_grab_frame(struct zms_pointer_grab *grab)
{
  Z_UNUSED(grab);
}

static void
zms_drag_grab_axis_discrete(
    struct zms_pointer_grab *grab, uint32_t axis, int32_t discrete)
{
  Z_UNUSED(grab);
  Z_UNUSED(axis);
  Z_UNUSED(discrete);
}

static void
zms_drag_grab_cancel(struct zms_pointer_grab *grab)
{
  struct zms_drag_grab *drag_grab = wl_container_of(grab, drag_grab, base);
  struct zms_data_device *data_device = grab->pointer->seat->data_device;
  struct zms_data_offer_proxy *data_offer_proxy = data_device->data_offer_proxy;

  if ((zms_data_device_set_focus(data_device, NULL, 0, 0) &
          ZMS_DATA_DEVICE_SET_FOCUS_LEFT) == 0 &&
      data_offer_proxy && data_device->is_dragging == false) {
    data_offer_proxy->destroy(data_offer_proxy);
  }

  zms_pointer_end_grab(grab->pointer);
  zms_drag_grab_destroy(drag_grab);
}

static const struct zms_pointer_grab_interface drag_grab_interface = {
    .focus = zms_drag_grab_focus,
    .motion_abs = zms_drag_grab_motion_abs,
    .button = zms_drag_grab_button,
    .axis = zms_drag_grab_axis,
    .frame = zms_drag_grab_frame,
    .axis_discrete = zms_drag_grab_axis_discrete,
    .cancel = zms_drag_grab_cancel,
};

static struct zms_drag_grab *
zms_drag_grab_create()
{
  struct zms_drag_grab *grab;

  grab = zalloc(sizeof *grab);
  if (grab == NULL) goto err;

  grab->base.interface = &drag_grab_interface;

  return grab;

err:
  return NULL;
}

static void
zms_drag_grab_destroy(struct zms_drag_grab *drag_grab)
{
  free(drag_grab);
}

ZMS_EXPORT bool
zms_drag_grab_start(struct zms_seat *seat)
{
  struct zms_drag_grab *grab;
  struct zms_pointer *pointer;

  pointer = seat->priv->pointer;
  if (pointer == NULL) return false;

  grab = zms_drag_grab_create();
  if (grab == NULL) return false;

  zms_pointer_start_grab(pointer, &grab->base);

  return true;
}
