#include "base.h"

#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "root.h"

ZMS_EXPORT struct zms_ui_base*
zms_ui_base_create(struct zms_ui_base* parent)
{
  struct zms_ui_base* ui_base;
  struct zms_opengl_component* component;

  ui_base = zalloc(sizeof *ui_base);
  if (ui_base == NULL) goto err;

  component =
      zms_opengl_component_create(parent->root->cuboid_window->virtual_object);
  if (component == NULL) goto err_component;

  ui_base->root = parent->root;
  ui_base->component = component;

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

  ui_base->root = root;
  ui_base->component = NULL;

  return ui_base;

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_base_destroy(struct zms_ui_base* ui_base)
{
  if (ui_base->component) zms_opengl_component_destroy(ui_base->component);
  free(ui_base);
}
