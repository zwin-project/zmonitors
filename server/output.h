#ifndef ZMONITORS_SERVER_OUTPUT_H
#define ZMONITORS_SERVER_OUTPUT_H

#include <pixman-1/pixman.h>
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
  struct zms_bgra* buffer;
  pixman_image_t* image;
  pixman_region32_t region;

  struct wl_list resource_list;
  struct wl_list view_list;

  struct zms_bgra* bg_buffer;
  pixman_image_t* bg_image;
};

void zms_output_map_view(struct zms_output* output, struct zms_view* view);

void zms_output_unmap_view(struct zms_output* output, struct zms_view* view);

void zms_output_update_view(struct zms_output* output, struct zms_view* view);

#endif  //  ZMONITORS_SERVER_OUTPUT_H
