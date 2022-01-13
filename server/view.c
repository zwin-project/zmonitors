#include "view.h"

#include <string.h>
#include <zmonitors-server.h>

#include "compositor.h"
#include "output.h"

ZMS_EXPORT struct zms_view*
zms_view_create(struct zms_surface* surface)
{
  struct zms_view* view;
  struct zms_view_private* priv;

  view = zalloc(sizeof *view);
  if (view == NULL) goto err;

  priv = zalloc(sizeof *priv);
  if (priv == NULL) goto err_priv;

  priv->pub = view;
  priv->surface = surface;
  priv->output = NULL;
  wl_list_init(&priv->link);
  priv->image = NULL;
  glm_vec2_zero(priv->origin);

  view->priv = priv;

  return view;

err_priv:
  free(view);

err:
  return NULL;
}

ZMS_EXPORT void
zms_view_destroy(struct zms_view* view)
{
  zms_output_unmap_view(view->priv->output, view);
  if (view->priv->image) pixman_image_unref(view->priv->image);
  zms_buffer_reference(&view->priv->buffer_ref, NULL);
  free(view->priv);
  free(view);
}

ZMS_EXPORT int
zms_view_commit(struct zms_view* view)
{
  void* data;
  int32_t width, height, stride;
  struct wl_shm_buffer* shm_buffer;
  struct zms_surface* surface = view->priv->surface;

  if (surface->pending.newly_attached == false) return -1;

  if (view->priv->image) {
    pixman_image_unref(view->priv->image);
    view->priv->image = NULL;
  }

  if (surface->pending.buffer == NULL) {
    glm_vec2_zero(view->priv->origin);
  } else {
    shm_buffer = wl_shm_buffer_get(surface->pending.buffer->resource);
    width = wl_shm_buffer_get_width(shm_buffer);
    height = wl_shm_buffer_get_height(shm_buffer);
    data = wl_shm_buffer_get_data(shm_buffer);
    stride = wl_shm_buffer_get_stride(shm_buffer);

    view->priv->image =
        pixman_image_create_bits(PIXMAN_a8r8g8b8, width, height, data, stride);
  }

  zms_buffer_reference(&view->priv->buffer_ref, surface->pending.buffer);

  return 0;
}

ZMS_EXPORT void
zms_view_set_origin(struct zms_view* view, float x, float y)
{
  view->priv->origin[0] = x;
  view->priv->origin[1] = y;
}

ZMS_EXPORT bool
zms_view_contains(struct zms_view* view, int x, int y, int* vx, int* vy)
{
  int view_x = x - view->priv->origin[0];
  int view_y = y - view->priv->origin[1];

  if (0 <= view_x && 0 <= view_y &&
      (uint32_t)view_x <= zms_view_get_width(view) &&
      (uint32_t)view_y <= zms_view_get_height(view)) {
    *vx = view_x;
    *vy = view_y;
    return true;
  }

  return false;
}
