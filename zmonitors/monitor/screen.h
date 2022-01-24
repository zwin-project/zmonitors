#ifndef ZMONITORS_MONITOR_SCREEN_H
#define ZMONITORS_MONITOR_SCREEN_H

#include "monitor.h"
#include "ui.h"

struct zms_screen {
  struct zms_ui_base *base;
  struct zms_monitor *monitor;
  struct zms_output *output;

  struct zms_opengl_component *component;
  struct zms_opengl_shader_program *shader;
  struct zms_opengl_vertex_buffer *vertex_buffer;
  struct zms_opengl_texture **textures;

  bool texture_changed;
  bool ray_focus;
  bool data_device_focus;
  uint32_t data_device_enter_serial;

  // for ray intersection
  vec3 v0, vx, vy;
};

struct zms_screen *zms_screen_create(struct zms_monitor *monitor);

void zms_screen_destroy(struct zms_screen *screen);

#endif  //  ZMONITORS_MONITOR_SCREEN_H
