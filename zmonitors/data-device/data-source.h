#ifndef ZMONITORS_DATA_DEVICE_DATA_SOURCE_H
#define ZMONITORS_DATA_DEVICE_DATA_SOURCE_H

#include <zmonitors-backend.h>
#include <zmonitors-server.h>

struct zms_data_source {
  struct wl_resource *resource;
  struct zms_backend_data_source *proxy;
};

struct zms_data_source *zms_data_source_create(
    struct wl_client *client, uint32_t id, struct zms_backend *backend);

#endif  //  ZMONITORS_DATA_DEVICE_DATA_SOURCE_H
