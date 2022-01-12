#include "monitor.h"

#include <cglm/cglm.h>
#include <stdio.h>
#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "monitor-internal.h"
#include "screen.h"
#include "ui.h"

#define DEFAULT_PPM 1000
#define CUBOID_DEPTH 0.01
#define CUBOID_PADDING 0.05

static void
ui_setup(struct zms_ui_base* ui_base)
{
  struct zms_monitor* monitor = ui_base->user_data;

  glm_vec3_copy(ui_base->half_size, monitor->screen->base->half_size);
  monitor->screen->base->half_size[0] -= CUBOID_PADDING;
  monitor->screen->base->half_size[1] -= CUBOID_PADDING;
}

static void
ui_teardown(struct zms_ui_base* ui_base)
{
  Z_UNUSED(ui_base);
}

static const struct zms_ui_base_interface ui_base_interface = {
    .setup = ui_setup,
    .teardown = ui_teardown,
};

ZMS_EXPORT struct zms_monitor*
zms_monitor_create(struct zms_backend* backend,
    struct zms_compositor* compositor, struct zms_screen_size size)
{
  struct zms_monitor* monitor;
  struct zms_ui_root* ui_root;
  struct zms_screen* screen;
  float ppm = DEFAULT_PPM;
  vec3 half_size;
  versor quaternion = GLM_QUAT_IDENTITY_INIT;

  half_size[0] = (float)size.width / 2 / ppm + CUBOID_PADDING;
  half_size[1] = (float)size.height / 2 / ppm + CUBOID_PADDING;
  half_size[2] = CUBOID_DEPTH;

  monitor = zalloc(sizeof *monitor);
  if (monitor == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  ui_root = zms_ui_root_create(
      monitor, &ui_base_interface, backend, half_size, quaternion);
  if (ui_root == NULL) goto err_ui_root;

  monitor->backend = backend;
  monitor->compositor = compositor;
  monitor->screen_size = size;
  monitor->ppm = ppm;
  monitor->ui_root = ui_root;

  screen = zms_screen_create(monitor);
  if (screen == NULL) goto err_screen;
  monitor->screen = screen;

  return monitor;

err_screen:
  zms_ui_root_destroy(ui_root);

err_ui_root:
  free(monitor);

err:
  return NULL;
}

ZMS_EXPORT void
zms_monitor_destroy(struct zms_monitor* monitor)
{
  zms_screen_destroy(monitor->screen);
  zms_ui_root_destroy(monitor->ui_root);
  free(monitor);
}
