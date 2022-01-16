#ifndef ZMONITORS_SERVER_DATA_DEVICE_H
#define ZMONITORS_SERVER_DATA_DEVICE_H

#include <wayland-server.h>

struct zms_data_device {
  struct wl_resource* resource;
};

struct zms_data_device* zms_data_device_create(
    struct wl_client* client, uint32_t id);

#endif  //  ZMONITORS_SERVER_DATA_DEVICE_H
