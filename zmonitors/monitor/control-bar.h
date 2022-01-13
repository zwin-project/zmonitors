#ifndef ZMONITORS_CONTROL_BAR_H
#define ZMONITORS_CONTROL_BAR_H

#include "monitor.h"
#include "ui.h"

struct zms_control_bar {
  struct zms_ui_base *base;
  struct zms_monitor *monitor;

  struct zms_opengl_component *component;
  struct zms_opengl_shader_program *shader;
  struct zms_opengl_vertex_buffer *vertex_buffer;
  struct zms_opengl_texture *texture;

  bool focus;
};

struct zms_control_bar *zms_control_bar_create(struct zms_monitor *monitor);

void zms_control_bar_destroy(struct zms_control_bar *control_bar);

#endif  //  ZMONITORS_CONTROL_BAR_H
