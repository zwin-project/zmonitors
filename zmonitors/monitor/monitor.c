#include "monitor.h"

#include <cglm/cglm.h>
#include <stdio.h>
#include <zmonitors-util.h>

struct zms_monitor {
  struct zms_screen_size screen_size;
  float ppm;  // pixels per meter
};

ZMS_EXPORT struct zms_monitor*
zms_monitor_create(struct zms_screen_size size)
{
  struct zms_monitor* monitor;

  monitor = zalloc(sizeof *monitor);
  if (monitor == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  monitor->screen_size = size;
  monitor->ppm = 1000;

  return monitor;

err:
  return NULL;
}

ZMS_EXPORT void
zms_monitor_destroy(struct zms_monitor* monitor)
{
  free(monitor);
}
