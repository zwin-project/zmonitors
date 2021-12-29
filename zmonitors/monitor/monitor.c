#include "monitor.h"

#include <cglm/cglm.h>
#include <stdio.h>
#include <zmonitors-backend.h>
#include <zmonitors-util.h>

struct zms_monitor {
  struct zms_backend* backend;
  struct zms_compositor* compositor;

  struct zms_screen_size screen_size;
  float ppm;  // pixels per meter

  // TODO: use higher level object
  struct zms_cuboid_window* cuboid_window;
};

ZMS_EXPORT struct zms_monitor*
zms_monitor_create(struct zms_backend* backend,
    struct zms_compositor* compositor, struct zms_screen_size size)
{
  struct zms_monitor* monitor;
  struct zms_cuboid_window* cuboid_window;
  vec3 half_size;
  versor quaternion = GLM_QUAT_IDENTITY_INIT;

  monitor = zalloc(sizeof *monitor);
  if (monitor == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  monitor->backend = backend;
  monitor->compositor = compositor;
  monitor->screen_size = size;
  monitor->ppm = 1000;

  half_size[0] = (float)monitor->screen_size.width / 2 / monitor->ppm;
  half_size[1] = (float)monitor->screen_size.height / 2 / monitor->ppm;
  half_size[2] = 0.05;
  cuboid_window = zms_cuboid_window_create(backend, half_size, quaternion);
  if (cuboid_window == NULL) goto err_cuboid_window;

  monitor->cuboid_window = cuboid_window;

  return monitor;

err_cuboid_window:
  free(monitor);

err:
  return NULL;
}

ZMS_EXPORT void
zms_monitor_destroy(struct zms_monitor* monitor)
{
  zms_cuboid_window_destroy(monitor->cuboid_window);
  free(monitor);
}
