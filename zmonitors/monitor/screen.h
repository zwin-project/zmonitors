#ifndef ZMONITORS_MONITOR_SCREEN_H
#define ZMONITORS_MONITOR_SCREEN_H

#include "monitor.h"
#include "ui.h"

struct zms_screen {
  struct zms_ui_base *base;
  struct zms_monitor *monitor;

  struct zms_opengl_component *component;
  struct zms_opengl_shader_program *shader;
  struct zms_opengl_vertex_buffer *vertex_buffer;
  struct zms_opengl_texture *texture;
};

struct zms_screen *zms_screen_create(struct zms_monitor *monitor);

void zms_screen_destroy(struct zms_screen *screen);

#endif  //  ZMONITORS_MONITOR_SCREEN_H
