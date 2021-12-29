#include "monitor.h"

#include <cglm/cglm.h>
#include <stdio.h>
#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "ui.h"

#define DEFAULT_PPM 1000
#define CUBOID_DEPTH 0.01

struct zms_monitor {
  struct zms_backend* backend;
  struct zms_compositor* compositor;

  struct zms_screen_size screen_size;
  float ppm;  // pixels per meter

  struct zms_ui_root* ui_root;
};

ZMS_EXPORT struct zms_monitor*
zms_monitor_create(struct zms_backend* backend,
    struct zms_compositor* compositor, struct zms_screen_size size)
{
  struct zms_monitor* monitor;
  struct zms_ui_root* ui_root;
  float ppm = DEFAULT_PPM;
  vec3 half_size;
  versor quaternion = GLM_QUAT_IDENTITY_INIT;

  monitor = zalloc(sizeof *monitor);
  if (monitor == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  half_size[0] = (float)size.width / 2 / ppm;
  half_size[1] = (float)size.height / 2 / ppm;
  half_size[2] = CUBOID_DEPTH;
  ui_root = zms_ui_root_create(backend, half_size, quaternion);
  if (ui_root == NULL) goto err_ui_root;

  monitor->backend = backend;
  monitor->compositor = compositor;
  monitor->screen_size = size;
  monitor->ppm = ppm;
  monitor->ui_root = ui_root;

  return monitor;

err_ui_root:
  free(monitor);

err:
  return NULL;
}

ZMS_EXPORT void
zms_monitor_destroy(struct zms_monitor* monitor)
{
  zms_ui_root_destroy(monitor->ui_root);
  free(monitor);
}
