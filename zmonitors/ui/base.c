#include "base.h"

#include <zmonitors-backend.h>
#include <zmonitors-util.h>

ZMS_EXPORT struct zms_ui_base*
zms_ui_base_create(struct zms_cuboid_window* cuboid_window)
{
  struct zms_ui_base* ui_base;

  ui_base = zalloc(sizeof *ui_base);
  if (ui_base == NULL) goto err;

  ui_base->cuboid_window = cuboid_window;

  return ui_base;

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_base_destroy(struct zms_ui_base* ui_base)
{
  free(ui_base);
}
