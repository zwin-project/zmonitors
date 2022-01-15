#ifndef ZMONITORS_SERVER_OUTPUT_H
#define ZMONITORS_SERVER_OUTPUT_H

#include <pixman-1/pixman.h>
#include <zmonitors-server.h>

#include "compositor.h"
#include "view-layer.h"

// ordered from top to bottom
enum zms_output_view_layer_index {
  ZMS_OUTPUT_CURSOR_LAYER_INDEX = 0,
  ZMS_OUTPUT_MAIN_LAYER_INDEX = 1,
};

#define ZMS_OUTPUT_VIEW_LAYER_COUNT 2

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
  struct zms_view_layer layers[ZMS_OUTPUT_VIEW_LAYER_COUNT];

  struct zms_bgra* bg_buffer;
  pixman_image_t* bg_image;
};

void zms_output_map_view(struct zms_output* output,
    struct zms_view* view /* must have image */,
    enum zms_output_view_layer_index layer_index);

void zms_output_unmap_view(struct zms_output* output, struct zms_view* view);

void zms_output_render(struct zms_output* output, pixman_region32_t* damage);

struct zms_view* zms_output_pick_view(
    struct zms_output* output, float x, float y, float* vx, float* vy);

#endif  //  ZMONITORS_SERVER_OUTPUT_H
