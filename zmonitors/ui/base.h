#ifndef ZMONITORS_UI_BASE_H
#define ZMONITORS_UI_BASE_H

#include <zmonitors-backend.h>

struct zms_ui_base {
  struct zms_cuboid_window *cuboid_window;
  struct zms_opengl_component *component;
};

struct zms_ui_base *zms_ui_base_create(struct zms_cuboid_window *cuboid_window);

void zms_ui_base_destroy(struct zms_ui_base *ui_base);

#endif  //  ZMONITORS_UI_BASE_H
