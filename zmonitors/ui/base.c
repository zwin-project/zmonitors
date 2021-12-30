#include "base.h"

#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "root.h"

ZMS_EXPORT struct zms_ui_base*
zms_ui_base_create(void* user_data,
    const struct zms_ui_base_interface* interface, struct zms_ui_base* parent)
{
  struct zms_ui_base* ui_base;
  struct zms_opengl_component* component;

  ui_base = zalloc(sizeof *ui_base);
  if (ui_base == NULL) goto err;

  component =
      zms_opengl_component_create(parent->root->cuboid_window->virtual_object);
  if (component == NULL) goto err_component;

  ui_base->user_data = user_data;
  ui_base->interface = interface;
  ui_base->root = parent->root;
  ui_base->parent = parent;
  wl_list_insert(&parent->children, &ui_base->link);
  wl_list_init(&ui_base->children);
  ui_base->component = component;
  ui_base->setup = false;

  return ui_base;

err_component:
  free(ui_base);

err:
  return NULL;
}

ZMS_EXPORT struct zms_ui_base*
zms_ui_base_create_root(struct zms_ui_root* root)
{
  struct zms_ui_base* ui_base;

  ui_base = zalloc(sizeof *ui_base);
  if (ui_base == NULL) goto err;

  ui_base->user_data = NULL;
  ui_base->interface = NULL;
  ui_base->root = root;
  ui_base->parent = NULL;
  wl_list_init(&ui_base->link);
  wl_list_init(&ui_base->children);
  ui_base->component = NULL;
  ui_base->setup = false;

  return ui_base;

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_base_destroy(struct zms_ui_base* ui_base)
{
  if (ui_base->setup) ui_base->interface->teardown(ui_base->user_data);
  wl_list_remove(&ui_base->link);
  if (ui_base->component) zms_opengl_component_destroy(ui_base->component);
  free(ui_base);
}

ZMS_EXPORT void
zms_ui_base_run_setup_phase(struct zms_ui_base* base)
{
  base->interface->setup(base->user_data);
  base->setup = true;

  struct zms_ui_base* child;
  wl_list_for_each(child, &base->children, link)
      zms_ui_base_run_setup_phase(child);
}

ZMS_EXPORT struct zms_opengl_component*
zms_ui_base_get_component(struct zms_ui_base* base)
{
  return base->component;
}
