#include "root.h"

#include <cglm/cglm.h>
#include <zmonitors-util.h>

#include "base.h"
#include "monitor.h"
#include "ui.h"

static void
frame_callback_handler(void* data, uint32_t time)
{
  struct zms_ui_root* root = data;

  zms_ui_base_run_frame_phase(root->base, time);

  switch (root->frame_state) {
    case ZMS_UI_FRAME_STATE_REPAINT_SCHEDULED:
      zms_ui_base_run_repaint_phase(root->base);
      zms_ui_root_commit(root);
      break;

    case ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME:
      root->frame_state = ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE;
      break;

    case ZMS_UI_FRAME_STATE_WAITING_CONTENT_UPDATE:
      assert(false && "not reached");
      break;
  }
}

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

  glm_vec3_copy(cuboid_window->half_size, root->base->half_size);
  zms_ui_base_run_setup_phase(root->base);

  zms_ui_root_commit(root);

  root->cuboid_window->configured = cuboid_window_configured_handler;
}

ZMS_EXPORT struct zms_ui_root*
zms_ui_root_create(void* user_data,
    const struct zms_ui_base_interface* interface, struct zms_backend* backend,
    vec3 half_size, versor quaternion)
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
  wl_list_init(&root->frame_callback_list);
  root->frame_state = ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME;

  base = zms_ui_base_create_root(root, user_data, interface);
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
  struct zms_frame_callback *frame_callback, *tmp;

  wl_list_for_each_safe(frame_callback, tmp, &root->frame_callback_list, link)
  {
    zms_frame_callback_destroy(frame_callback);
  }

  zms_ui_base_destroy(root->base);
  zms_cuboid_window_destroy(root->cuboid_window);
  free(root);
}

ZMS_EXPORT void
zms_ui_root_commit(struct zms_ui_root* root)
{
  struct zms_frame_callback* frame_callback;

  root->frame_state = ZMS_UI_FRAME_STATE_WAITING_NEXT_FRAME;
  frame_callback = zms_frame_callback_create(
      root->cuboid_window->virtual_object, root, frame_callback_handler);

  wl_list_insert(&root->frame_callback_list, &frame_callback->link);

  zms_cuboid_window_commit(root->cuboid_window);
  zms_backend_flush(root->cuboid_window->backend);
}
