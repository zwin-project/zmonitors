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
  glm_quat_identity(ui_base->quaternion);
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
  glm_quat_identity(ui_base->quaternion);
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
  switch (ui_base->root->frame_state) {
    case ZMS_UI_FRAME_STATE_REPAINT_SCHEDULED:
      // fall through
    case ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME:
      ui_base->root->frame_state = ZMS_UI_FRAME_STATE_REPAINT_SCHEDULED;
      break;

    case ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE:
      zms_ui_base_run_repaint_phase(ui_base->root->base);
      zms_ui_root_commit(ui_base->root);
      break;
  }
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
zms_ui_base_run_repaint_phase(struct zms_ui_base* ui_base)
{
  ui_base->interface->repaint(ui_base);

  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
      zms_ui_base_run_repaint_phase(child);
}

ZMS_EXPORT void
zms_ui_base_run_frame_phase(struct zms_ui_base* ui_base, uint32_t time)
{
  ui_base->interface->frame(ui_base, time);

  struct zms_ui_base* child;
  wl_list_for_each(child, &ui_base->children, link)
      zms_ui_base_run_frame_phase(child, time);
}
