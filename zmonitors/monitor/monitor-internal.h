#ifndef ZMONITORS_MONITOR_MONITOR_INTERNAL_H
#define ZMONITORS_MONITOR_MONITOR_INTERNAL_H

struct zms_monitor {
  struct zms_backend* backend;
  struct zms_compositor* compositor;
  struct zms_output* output;

  struct zms_screen_size screen_size;
  float ppm;  // pixels per meter

  struct zms_ui_root* ui_root;
  struct zms_screen* screen;
};

#endif  //  ZMONITORS_MONITOR_MONITOR_INTERNAL_H
