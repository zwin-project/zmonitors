#ifndef ZMONITORS_DATA_DEVICE_DATA_DEVICE_H
#define ZMONITORS_DATA_DEVICE_DATA_DEVICE_H

#include <zmonitors-backend.h>
#include <zmonitors-server.h>

struct zms_data_device {
  struct zms_backend *backend;
  struct zms_compositor *compositor;

  struct wl_list resource_list;
};

struct zms_data_device *zms_data_device_create(
    struct zms_compositor *compositor, struct zms_backend *backend);

void zms_data_device_destroy(struct zms_data_device *data_device);

struct wl_resource *zms_data_device_create_resource(
    struct zms_data_device *data_device, struct wl_client *client, uint32_t id);

#endif  //  ZMONITORS_DATA_DEVICE_DATA_DEVICE_H
