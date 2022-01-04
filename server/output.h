#ifndef ZMONITORS_SERVER_OUTPUT_H
#define ZMONITORS_SERVER_OUTPUT_H

#include <zmonitors-server.h>

#include "compositor.h"

struct zms_output_private {
  void* user_data;
  const struct zms_output_interface* interface;

  struct wl_global* global;
  struct zms_compositor* compositor;

  struct zms_screen_size size;
  vec2 physical_size;
  char* manufacturer;
  char* model;

  int fd;

  struct wl_list resource_list;
};

#endif  //  ZMONITORS_SERVER_OUTPUT_H
