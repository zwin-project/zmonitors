#ifndef ZMONITORS_SERVER_OUTPUT_H
#define ZMONITORS_SERVER_OUTPUT_H

#include <zmonitors.h>

#include "compositor.h"

struct zms_output_private {
  struct wl_global* global;
  struct zms_compositor* compositor;

  struct wl_list resource_list;
};

struct zms_output* zms_output_create(struct zms_compositor* compositor);

void zms_output_destroy(struct zms_output* output);

#endif  //  ZMONITORS_SERVER_OUTPUT_H
