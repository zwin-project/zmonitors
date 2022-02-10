#include "monitor.h"

#include <cglm/cglm.h>
#include <stdio.h>
#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "monitor-internal.h"
#include "screen.h"
#include "ui.h"

#define DEFAULT_PPM 2000
#define CUBOID_DEPTH 0.01
#define CUBOID_PADDING 0.05
#define CONTROL_BAR_PADDING 0.02
#define CONTROL_BAR_HEIGHT 0.02
#define CONTROL_BAR_WIDTH 0.4

static void
ui_setup_geometry(struct zms_ui_base* ui_base)
{
  struct zms_monitor* monitor = ui_base->user_data;

  glm_vec3_copy(ui_base->half_size, monitor->screen->base->half_size);
  monitor->screen->base->half_size[0] -= CUBOID_PADDING;
  monitor->screen->base->half_size[1] -=
      CUBOID_PADDING + (CONTROL_BAR_HEIGHT + CONTROL_BAR_PADDING) / 2;
  monitor->screen->base->position[1] =
      (CONTROL_BAR_HEIGHT + CONTROL_BAR_PADDING) / 2;

  monitor->control_bar->base->half_size[0] = CONTROL_BAR_WIDTH / 2;
  monitor->control_bar->base->half_size[1] = CONTROL_BAR_HEIGHT / 2;
  monitor->control_bar->base->position[1] =
      -ui_base->half_size[1] + CUBOID_PADDING + CONTROL_BAR_HEIGHT / 2;
}

static void
ui_setup(struct zms_ui_base* ui_base)
{
  ui_setup_geometry(ui_base);
}

static void
ui_teardown(struct zms_ui_base* ui_base)
{
  Z_UNUSED(ui_base);
}

static void
ui_reconfigure(struct zms_ui_base* ui_base)
{
  ui_setup_geometry(ui_base);
}

static bool
cuboid_window_moved(struct zms_ui_base* ui_base, vec3 face_direction)
{
  Z_UNUSED(ui_base);
  vec3 front = {0.0f, 0.0f, 1.0f};
  versor quaternion;

  glm_quat_from_vecs(front, face_direction, quaternion);

  zms_cuboid_window_rotate(ui_base->root->cuboid_window, quaternion);

  return true;
}

static const struct zms_ui_base_interface ui_base_interface = {
    .setup = ui_setup,
    .teardown = ui_teardown,
    .reconfigure = ui_reconfigure,
    .cuboid_window_moved = cuboid_window_moved,
};

ZMS_EXPORT struct zms_monitor*
zms_monitor_create(struct zms_backend* backend,
    struct zms_compositor* compositor, struct zms_screen_size size)
{
  struct zms_monitor* monitor;
  struct zms_ui_root* ui_root;
  struct zms_screen* screen;
  struct zms_control_bar* control_bar;
  float ppm = DEFAULT_PPM;
  vec3 half_size;
  versor quaternion = GLM_QUAT_IDENTITY_INIT;

  half_size[0] = (float)size.width / 2 / ppm + CUBOID_PADDING;
  half_size[1] = (float)size.height / 2 / ppm + CUBOID_PADDING +
                 (CONTROL_BAR_HEIGHT + CONTROL_BAR_PADDING) / 2;
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

  control_bar = zms_control_bar_create(monitor);
  if (control_bar == NULL) goto err_control_bar;
  monitor->control_bar = control_bar;

  return monitor;

err_control_bar:
  zms_screen_destroy(screen);

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
  zms_control_bar_destroy(monitor->control_bar);
  zms_screen_destroy(monitor->screen);
  zms_ui_root_destroy(monitor->ui_root);
  free(monitor);
}
