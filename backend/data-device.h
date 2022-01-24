#ifndef ZMONITORS_BACKEND_DATA_DEVICE_H
#define ZMONITORS_BACKEND_DATA_DEVICE_H

#include <zigen-client-protocol.h>
#include <zmonitors-util.h>

#include "backend.h"

struct zms_backend_data_device {
  struct zgn_data_device* proxy;
  struct zms_backend* backend;
  struct zms_weak_ref focus_virtual_object_ref;
};

struct zms_backend_data_device* zms_backend_data_device_create(
    struct zms_backend* backend);

void zms_backend_data_device_destroy(
    struct zms_backend_data_device* data_device);

#endif  //  ZMONITORS_BACKEND_DATA_DEVICE_H
