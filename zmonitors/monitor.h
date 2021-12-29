#ifndef ZMONITORS_MONITOR_H
#define ZMONITORS_MONITOR_H

#include <stdint.h>
#include <zmonitors-backend.h>
#include <zmonitors-server.h>
#include <zmonitors-types.h>

struct zms_monitor;

struct zms_monitor* zms_monitor_create(struct zms_backend* backend,
    struct zms_compositor* compositor, struct zms_screen_size size);

void zms_monitor_destroy(struct zms_monitor* monitor);

#endif  //  ZMONITORS_MONITOR_H
