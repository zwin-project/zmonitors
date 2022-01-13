#ifndef ZMONITORS_SERVER_VIEW_H
#define ZMONITORS_SERVER_VIEW_H

#include <pixman-1/pixman.h>
#include <stdbool.h>
#include <wayland-server.h>
#include <zmonitors-server.h>

#include "buffer.h"
#include "surface.h"

struct zms_view_private {
  struct zms_view* pub;

  struct zms_surface* surface; /* nonnull */
  struct zms_buffer_ref buffer_ref;

  /* The output that this view is mapped.
   * Be careful of its data consistency with zms_view_private.link.
   * managed by zms_output. nullable. */
  struct zms_output* output;
  /* link with self->output->priv->view_list when self->output is not null.
   * self-pointed link when self->output is null.
   * managed by zms_output. */
  struct wl_list link;

  pixman_image_t* image; /* nullable */
  vec2 origin;
};

struct zms_view* zms_view_create(struct zms_surface* surface);

void zms_view_destroy(struct zms_view* view);

int zms_view_commit(struct zms_view* view);

void zms_view_set_origin(struct zms_view* view, float x, float y);

static inline bool
zms_view_is_mapped(struct zms_view* view)
{
  return view->priv->output != NULL;
}

static inline uint32_t
zms_view_get_width(struct zms_view* view)
{
  if (view->priv->image == NULL)
    return 0;
  else
    return pixman_image_get_width(view->priv->image);
}

static inline uint32_t
zms_view_get_height(struct zms_view* view)
{
  if (view->priv->image == NULL)
    return 0;
  else
    return pixman_image_get_height(view->priv->image);
}

bool zms_view_contains(struct zms_view* view, int x, int y, int* vx, int* vy);

#endif  //  ZMONITORS_SERVER_VIEW_H
