#ifndef ZMONITORS_UI_BASE_H
#define ZMONITORS_UI_BASE_H

#include <zmonitors-backend.h>

#include "ui.h"

struct zms_ui_base {
  struct zms_ui_root *root;
  struct zms_opengl_component *component;
};

struct zms_ui_base *zms_ui_base_create_root(struct zms_ui_root *root);

#endif  //  ZMONITORS_UI_BASE_H
