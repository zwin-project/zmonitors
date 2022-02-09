#include "root.h"

#include <cglm/cglm.h>
#include <zmonitors-backend.h>

#include "base.h"
#include "monitor.h"
#include "ui.h"

enum zms_ui_frame_state {
  ZMS_UI_FRAME_STATE_REPAINT_SCHEDULED = 0,
  ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME,
  ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE,
};

static void zms_ui_root_commit(struct zms_ui_root* root);

static void
zms_ui_root_ray_enter(void* data, uint32_t serial, vec3 origin, vec3 direction)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_ray_enter(root->base, serial, origin, direction);
}

static void
zms_ui_root_ray_leave(void* data, uint32_t serial)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_ray_leave(root->base, serial);
}

static void
zms_ui_root_ray_motion(void* data, uint32_t time, vec3 origin, vec3 direction)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_ray_motion(root->base, time, origin, direction);
}

static void
zms_ui_root_ray_button(
    void* data, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_ray_button(root->base, serial, time, button, state);
}

static void
zms_ui_root_ray_axis(void* data, uint32_t time, uint32_t axis, float value)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_ray_axis(root->base, time, axis, value);
}

static void
zms_ui_root_ray_frame(void* data)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_ray_frame(root->base);
}

static void
zms_ui_root_ray_axis_discrete(void* data, uint32_t axis, int32_t discrete)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_ray_axis_discrete(root->base, axis, discrete);
}

static void
zms_ui_root_data_device_enter(void* data, uint32_t serial, vec3 origin,
    vec3 direction, void* data_offer_user_data)
{
  struct zms_ui_root* root = data;
  struct zms_data_offer_proxy* data_offer_proxy = data_offer_user_data;
  zms_ui_base_propagate_data_device_enter(
      root->base, serial, origin, direction, data_offer_proxy);
}

static void
zms_ui_root_data_device_leave(void* data)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_data_device_leave(root->base);
}

static void
zms_ui_root_data_device_motion_abs(
    void* data, uint32_t time, vec3 origin, vec3 direction)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_data_device_motion_abs(
      root->base, time, origin, direction);
}

static void
zms_ui_root_data_device_drop(void* data)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_data_device_drop(root->base);
}

static void
zms_ui_root_keyboard_enter(void* data, uint32_t serial, struct wl_array* keys)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_keyboard_enter(root->base, serial, keys);
}

static void
zms_ui_root_keyboard_leave(void* data, uint32_t serial)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_keyboard_leave(root->base, serial);
}

static void
zms_ui_root_keyboard_key(
    void* data, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_keyboard_key(root->base, serial, time, key, state);
}

static void
zms_ui_root_keyboard_modifiers(void* data, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
    uint32_t group)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_keyboard_modifiers(
      root->base, serial, mods_depressed, mods_latched, mods_locked, group);
}

static void
zms_ui_root_cuboid_window_moved(void* data, vec3 face_direction)
{
  struct zms_ui_root* root = data;
  zms_ui_base_propagate_cuboid_window_moved(root->base, face_direction);
}

static const struct zms_cuboid_window_interface cuboid_window_interface = {
    {
        .ray_enter = zms_ui_root_ray_enter,
        .ray_leave = zms_ui_root_ray_leave,
        .ray_motion = zms_ui_root_ray_motion,
        .ray_button = zms_ui_root_ray_button,
        .ray_axis = zms_ui_root_ray_axis,
        .ray_frame = zms_ui_root_ray_frame,
        .ray_axis_discrete = zms_ui_root_ray_axis_discrete,
        .data_device_enter = zms_ui_root_data_device_enter,
        .data_device_leave = zms_ui_root_data_device_leave,
        .data_device_motion_abs = zms_ui_root_data_device_motion_abs,
        .data_device_drop = zms_ui_root_data_device_drop,
        .keyboard_enter = zms_ui_root_keyboard_enter,
        .keyboard_leave = zms_ui_root_keyboard_leave,
        .keyboard_key = zms_ui_root_keyboard_key,
        .keyboard_modifiers = zms_ui_root_keyboard_modifiers,
    },
    .moved = zms_ui_root_cuboid_window_moved,
};

static void
cuboid_window_configured_handler(
    void* data, struct zms_cuboid_window* cuboid_window)
{
  Z_UNUSED(cuboid_window);
  struct zms_ui_root* root = data;
  zms_ui_base_run_reconfigure_phase(root->base);

  zms_ui_base_schedule_repaint(root->base);
}

static void
cuboid_window_first_configured_handler(
    void* data, struct zms_cuboid_window* cuboid_window)
{
  Z_UNUSED(cuboid_window);
  struct zms_ui_root* root = data;

  glm_vec3_copy(cuboid_window->half_size, root->base->half_size);
  zms_ui_base_run_setup_phase(root->base);

  zms_ui_root_commit(root);

  root->cuboid_window->configured = cuboid_window_configured_handler;
}

ZMS_EXPORT struct zms_ui_root*
zms_ui_root_create(void* user_data,
    const struct zms_ui_base_interface* interface, struct zms_backend* backend,
    vec3 half_size, versor quaternion)
{
  struct zms_ui_root* root;
  struct zms_cuboid_window* cuboid_window;
  struct zms_ui_base* base;

  root = zalloc(sizeof *root);
  if (root == NULL) goto err;

  cuboid_window = zms_cuboid_window_create(
      root, &cuboid_window_interface, backend, half_size, quaternion);
  if (cuboid_window == NULL) goto err_cuboid_window;
  cuboid_window->configured = cuboid_window_first_configured_handler;

  root->cuboid_window = cuboid_window;
  wl_list_init(&root->frame_callback_list);
  root->frame_state = ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE;

  base = zms_ui_base_create_root(root, user_data, interface);
  if (base == NULL) goto err_base;
  root->base = base;

  return root;

err_base:
  zms_cuboid_window_destroy(cuboid_window);

err_cuboid_window:
  free(root);

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_root_destroy(struct zms_ui_root* root)
{
  struct zms_frame_callback *frame_callback, *tmp;

  wl_list_for_each_safe(frame_callback, tmp, &root->frame_callback_list, link)
  {
    zms_frame_callback_destroy(frame_callback);
  }

  zms_ui_base_destroy(root->base);
  zms_cuboid_window_destroy(root->cuboid_window);
  free(root);
}

// repainting schedule

static void
frame_callback_handler(void* data, uint32_t time)
{
  struct zms_ui_root* root = data;

  zms_ui_base_run_frame_phase(root->base, time);

  switch (root->frame_state) {
    case ZMS_UI_FRAME_STATE_REPAINT_SCHEDULED:
      zms_ui_base_run_repaint_phase(root->base);
      root->frame_state = ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE;
      zms_ui_root_commit(root);
      break;

    case ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME:
      root->frame_state = ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE;
      break;

    case ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE:
      assert(false && "not reached");
      break;
  }
}

static void
zms_ui_root_commit(struct zms_ui_root* root)
{
  if (root->frame_state == ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE) {
    struct zms_frame_callback* frame_callback;

    root->frame_state = ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME;
    frame_callback = zms_frame_callback_create(
        root->cuboid_window->virtual_object, root, frame_callback_handler);

    wl_list_insert(&root->frame_callback_list, &frame_callback->link);
  }

  zms_cuboid_window_commit(root->cuboid_window);
  zms_backend_flush(root->cuboid_window->backend);
}

ZMS_EXPORT void
zms_ui_root_schedule_repaint(struct zms_ui_root* root)
{
  switch (root->frame_state) {
    case ZMS_UI_FRAME_STATE_REPAINT_SCHEDULED:
      // fall through
    case ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME:
      root->frame_state = ZMS_UI_FRAME_STATE_REPAINT_SCHEDULED;
      break;

    case ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE:
      zms_ui_base_run_repaint_phase(root->base);
      zms_ui_root_commit(root);
      break;
  }
}
