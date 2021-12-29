#include "base.h"

#include <zmonitors-backend.h>
#include <zmonitors-util.h>

ZMS_EXPORT struct zms_ui_base*
zms_ui_base_create(struct zms_cuboid_window* cuboid_window)
{
  struct zms_ui_base* ui_base;
  struct zms_opengl_component* component;

  ui_base = zalloc(sizeof *ui_base);
  if (ui_base == NULL) goto err;

  component = zms_opengl_component_create(cuboid_window->virtual_object);
  if (component == NULL) goto err_component;

  ui_base->cuboid_window = cuboid_window;
  ui_base->component = component;

  return ui_base;

err_component:
  free(ui_base);

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_base_destroy(struct zms_ui_base* ui_base)
{
  zms_opengl_component_destroy(ui_base->component);
  free(ui_base);
}
