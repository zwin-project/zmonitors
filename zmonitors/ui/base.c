#include "base.h"

#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "root.h"

ZMS_EXPORT struct zms_ui_base*
zms_ui_base_create(void* user_data,
    const struct zms_ui_base_interface* interface, struct zms_ui_base* parent)
{
  struct zms_ui_base* ui_base;

  ui_base = zalloc(sizeof *ui_base);
  if (ui_base == NULL) goto err;

  ui_base->user_data = user_data;
  ui_base->interface = interface;
  ui_base->root = parent->root;
  ui_base->parent = parent;
  wl_list_insert(&parent->children, &ui_base->link);
  wl_list_init(&ui_base->children);
  glm_vec3_zero(ui_base->position);
  glm_vec3_zero(ui_base->half_size);
  ui_base->setup = false;

  return ui_base;

err:
  return NULL;
}

ZMS_EXPORT struct zms_ui_base*
zms_ui_base_create_root(struct zms_ui_root* root, void* user_data,
    const struct zms_ui_base_interface* interface)
{
  struct zms_ui_base* ui_base;

  ui_base = zalloc(sizeof *ui_base);
  if (ui_base == NULL) goto err;

  ui_base->user_data = user_data;
  ui_base->interface = interface;
  ui_base->root = root;
  ui_base->parent = NULL;
  wl_list_init(&ui_base->link);
  wl_list_init(&ui_base->children);
  glm_vec3_zero(ui_base->position);
  glm_vec3_zero(ui_base->half_size);
  ui_base->setup = false;

  return ui_base;

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_base_destroy(struct zms_ui_base* ui_base)
{
  if (ui_base->setup) ui_base->interface->teardown(ui_base);
  wl_list_remove(&ui_base->link);
  free(ui_base);
}

ZMS_EXPORT void
zms_ui_base_schedule_repaint(struct zms_ui_base* ui_base)
{
  zms_ui_root_schedule_repaint(ui_base->root);
}

ZMS_EXPORT void
zms_ui_base_run_setup_phase(struct zms_ui_base* ui_base)
{
  ui_base->interface->setup(ui_base);
  ui_base->setup = true;

  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
      zms_ui_base_run_setup_phase(child);
}

ZMS_EXPORT void
zms_ui_base_run_reconfigure_phase(struct zms_ui_base* ui_base)
{
  ui_base->interface->reconfigure(ui_base);

  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
      zms_ui_base_run_reconfigure_phase(child);
}

ZMS_EXPORT void
zms_ui_base_run_repaint_phase(struct zms_ui_base* ui_base)
{
  if (ui_base->interface->repaint) ui_base->interface->repaint(ui_base);

  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
      zms_ui_base_run_repaint_phase(child);
}

ZMS_EXPORT void
zms_ui_base_run_frame_phase(struct zms_ui_base* ui_base, uint32_t time)
{
  if (ui_base->interface->frame) ui_base->interface->frame(ui_base, time);

  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
      zms_ui_base_run_frame_phase(child, time);
}

ZMS_EXPORT bool
zms_ui_base_propagate_ray_enter(
    struct zms_ui_base* ui_base, uint32_t serial, vec3 origin, vec3 direction)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_ray_enter(child, serial, origin, direction))
      return false;
  }

  if (ui_base->interface->ray_enter)
    return ui_base->interface->ray_enter(ui_base, serial, origin, direction);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_ray_leave(struct zms_ui_base* ui_base, uint32_t serial)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_ray_leave(child, serial)) return false;
  }

  if (ui_base->interface->ray_leave)
    return ui_base->interface->ray_leave(ui_base, serial);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_ray_motion(
    struct zms_ui_base* ui_base, uint32_t time, vec3 origin, vec3 direction)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_ray_motion(child, time, origin, direction))
      return false;
  }

  if (ui_base->interface->ray_motion)
    return ui_base->interface->ray_motion(ui_base, time, origin, direction);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_ray_button(struct zms_ui_base* ui_base, uint32_t serial,
    uint32_t time, uint32_t button, uint32_t state)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_ray_button(child, serial, time, button, state))
      return false;
  }

  if (ui_base->interface->ray_button)
    return ui_base->interface->ray_button(ui_base, serial, time, button, state);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_data_device_enter(struct zms_ui_base* ui_base,
    uint32_t serial, vec3 origin, vec3 direction,
    struct zms_data_offer_proxy* data_offer_proxy)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_data_device_enter(
            child, serial, origin, direction, data_offer_proxy))
      return false;
  }

  if (ui_base->interface->data_device_enter)
    return ui_base->interface->data_device_enter(
        ui_base, serial, origin, direction, data_offer_proxy);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_data_device_leave(struct zms_ui_base* ui_base)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_data_device_leave(child)) return false;
  }

  if (ui_base->interface->data_device_leave)
    return ui_base->interface->data_device_leave(ui_base);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_data_device_motion_abs(
    struct zms_ui_base* ui_base, uint32_t time, vec3 origin, vec3 direction)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_data_device_motion_abs(
            child, time, origin, direction))
      return false;
  }

  if (ui_base->interface->data_device_motion_abs)
    return ui_base->interface->data_device_motion_abs(
        ui_base, time, origin, direction);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_data_device_drop(struct zms_ui_base* ui_base)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_data_device_drop(child)) return false;
  }

  if (ui_base->interface->data_device_drop)
    return ui_base->interface->data_device_drop(ui_base);

  return true;
}

ZMS_EXPORT bool
zms_ui_base_propagate_cuboid_window_moved(
    struct zms_ui_base* ui_base, vec3 face_direction)
{
  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
  {
    if (!zms_ui_base_propagate_cuboid_window_moved(child, face_direction))
      return false;
  }

  if (ui_base->interface->cuboid_window_moved)
    return ui_base->interface->cuboid_window_moved(ui_base, face_direction);

  return true;
}
