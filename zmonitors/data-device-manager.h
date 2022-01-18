#ifndef ZMONITORS_DATA_DEVICE_MANAGER_H
#define ZMONITORS_DATA_DEVICE_MANAGER_H

#include <zmonitors-backend.h>
#include <zmonitors-server.h>

struct zms_data_device_manager;

struct zms_data_device_manager* zms_data_device_manager_create(
    struct zms_compositor* compositor, struct zms_backend* backend);

void zms_data_device_manager_destroy(
    struct zms_data_device_manager* data_device_manager);

#endif  //  ZMONITORS_DATA_DEVICE_MANAGER_H
