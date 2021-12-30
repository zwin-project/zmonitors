#include "root.h"

#include <cglm/cglm.h>
#include <zmonitors-util.h>

#include "monitor.h"
#include "ui.h"

static void
cuboid_window_configured_handler(
    void* data, struct zms_cuboid_window* cuboid_window)
{
  // TODO: update geom
  zms_log("[ui root] warn: configured handler not implemented yet\n");
  Z_UNUSED(data);
  Z_UNUSED(cuboid_window);
}

static void
cuboid_window_first_configured_handler(
    void* data, struct zms_cuboid_window* cuboid_window)
{
  Z_UNUSED(cuboid_window);
  struct zms_ui_root* root = data;
  struct zms_ui_base* child;

  wl_list_for_each(child, &root->base->children, link)
      zms_ui_base_run_setup_phase(child);

  zms_cuboid_window_commit(cuboid_window);

  root->cuboid_window->configured = cuboid_window_configured_handler;
}

ZMS_EXPORT struct zms_ui_root*
zms_ui_root_create(
    struct zms_backend* backend, vec3 half_size, versor quaternion)
{
  struct zms_ui_root* root;
  struct zms_cuboid_window* cuboid_window;
  struct zms_ui_base* base;

  root = zalloc(sizeof *root);
  if (root == NULL) goto err;

  cuboid_window =
      zms_cuboid_window_create(root, backend, half_size, quaternion);
  if (cuboid_window == NULL) goto err_cuboid_window;
  cuboid_window->configured = cuboid_window_first_configured_handler;

  root->cuboid_window = cuboid_window;

  base = zms_ui_base_create_root(root);
  if (base == NULL) goto err_base;
  root->base = base;

  return root;

err_base:
  zms_cuboid_window_destroy(cuboid_window);

err_cuboid_window:
  free(root);

err:
  return NULL;
}

ZMS_EXPORT void
zms_ui_root_destroy(struct zms_ui_root* root)
{
  zms_ui_base_destroy(root->base);
  zms_cuboid_window_destroy(root->cuboid_window);
  free(root);
}

ZMS_EXPORT struct zms_ui_base*
zms_ui_root_get_base(struct zms_ui_root* root)
{
  return root->base;
}
