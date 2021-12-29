#include <zmonitors-util.h>

#include "base.h"
#include "ui.h"

struct zms_ui_paper {
  struct zms_ui_base* base;
};

ZMS_EXPORT struct zms_ui_paper*
zms_ui_paper_create(struct zms_cuboid_window* cuboid_window)
{
  struct zms_ui_paper* paper;
  struct zms_ui_base* base;

  paper = zalloc(sizeof *paper);
  if (paper == NULL) goto err;

  base = zms_ui_base_create(cuboid_window);
  if (base == NULL) goto err_base;

  paper->base = base;

  return paper;

err_base:
  free(paper);

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_paper_destroy(struct zms_ui_paper* paper)
{
  zms_ui_base_destroy(paper->base);
  free(paper);
}
