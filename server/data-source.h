#ifndef ZMONITORS_DATA_SOURCE_H
#define ZMONITORS_DATA_SOURCE_H

#include <wayland-server.h>

struct zms_data_source {
  struct wl_resource *resource;
};

struct zms_data_source *zms_data_source_create(
    struct wl_client *client, uint32_t id);

#endif  //  ZMONITORS_DATA_SOURCE_H
