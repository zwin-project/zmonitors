#ifndef ZMONITORS_MONITOR_H
#define ZMONITORS_MONITOR_H

#include <stdint.h>
#include <zmonitors-types.h>

struct zms_monitor;

struct zms_monitor* zms_monitor_create(struct zms_screen_size size);

void zms_monitor_destroy(struct zms_monitor* monitor);

#endif  //  ZMONITORS_MONITOR_H
