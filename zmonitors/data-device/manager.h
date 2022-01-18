#ifndef ZMONITORS_DATA_DEVICE_MANAGER_INTERNAL_H
#define ZMONITORS_DATA_DEVICE_MANAGER_INTERNAL_H

#include <wayland-server.h>

#include "data-device.h"

struct zms_data_device_manager {
  struct wl_global *global;
  struct zms_compositor *compositor;
  struct zms_backend *backend;

  struct zms_data_device *data_device;
};

#endif  //  ZMONITORS_DATA_DEVICE_MANAGER_INTERNAL_H
