#ifndef ZMONITORS_SERVER_DATA_DEVICE_MANAGER_H
#define ZMONITORS_SERVER_DATA_DEVICE_MANAGER_H

#include <wayland-server.h>
#include <zmonitors-server.h>

struct zms_data_device_manager {
  struct wl_global* global;
  struct zms_compositor* compositor;
};

struct zms_data_device_manager* zms_data_device_manager_create(
    struct zms_compositor* compositor);

void zms_data_device_manager_destroy(
    struct zms_data_device_manager* data_device_manager);

#endif  //  ZMONITORS_SERVER_DATA_DEVICE_MANAGER_H
