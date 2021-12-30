#ifndef ZMONITORS_UI_BASE_H
#define ZMONITORS_UI_BASE_H

#include <wayland-util.h>
#include <zmonitors-backend.h>

#include "ui.h"

struct zms_ui_base {
  void *user_data;
  const struct zms_ui_base_interface *interface;

  struct zms_ui_root *root;
  struct zms_ui_base *parent;
  struct wl_list link;  // -> zms_ui_base.children
  struct wl_list children;

  struct zms_opengl_component *component;

  bool setup;
};

struct zms_ui_base *zms_ui_base_create_root(struct zms_ui_root *root);

void zms_ui_base_run_setup_phase(struct zms_ui_base *base);

#endif  //  ZMONITORS_UI_BASE_H
