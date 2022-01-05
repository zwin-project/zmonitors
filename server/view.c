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
  priv->buffer = NULL;
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
  free(view->priv->buffer);
  zms_output_unmap_view(view->priv->output, view);
  free(view->priv);
  free(view);
}

ZMS_EXPORT int
zms_view_commit(struct zms_view* view)
{
  void* data;
  size_t current_size, next_size;
  int32_t current_height, current_stride;
  struct wl_shm_buffer* shm_buffer;
  struct zms_surface* surface = view->priv->surface;

  if (surface->pending.newly_attached == false) return -1;
  if (surface->pending.buffer_resource == NULL) {
    free(view->priv->buffer);
    view->priv->buffer = NULL;
    view->priv->width = 0;
    view->priv->height = 0;
    view->priv->stride = 0;
    glm_vec2_zero(view->priv->origin);
    return 0;
  }

  current_height = view->priv->height;
  current_stride = view->priv->stride;

  shm_buffer = wl_shm_buffer_get(surface->pending.buffer_resource);
  view->priv->width = wl_shm_buffer_get_width(shm_buffer);
  view->priv->height = wl_shm_buffer_get_height(shm_buffer);
  view->priv->stride = wl_shm_buffer_get_stride(shm_buffer);

  next_size = view->priv->stride * view->priv->height;

  if (view->priv->buffer == NULL) {
    view->priv->buffer = malloc(next_size);
  } else if (current_height != view->priv->height ||
             current_stride != view->priv->width) {
    current_size = current_stride * current_height;

    if (current_size > next_size) {
      view->priv->buffer = realloc(view->priv->buffer, next_size);
    } else if (current_size < next_size) {
      free(view->priv->buffer);
      view->priv->buffer = malloc(next_size);
    }
  }

  data = wl_shm_buffer_get_data(shm_buffer);
  wl_shm_buffer_begin_access(shm_buffer);
  memcpy(view->priv->buffer, data, next_size);
  wl_shm_buffer_end_access(shm_buffer);

  return 0;
}
