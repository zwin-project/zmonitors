#ifndef ZMONITORS_SERVER_VIEW_H
#define ZMONITORS_SERVER_VIEW_H

#include <stdbool.h>
#include <wayland-server.h>
#include <zmonitors-server.h>

#include "surface.h"

struct zms_view_private {
  struct zms_view* pub;

  struct zms_surface* surface; /* nonnull */

  /* The output that this view is mapped.
   * Be careful of its data consistency with zms_view_private.link.
   * managed by zms_output. nullable. */
  struct zms_output* output;
  /* self->output->priv->view_list when self->output is not null.
   * self-pointed link when self->output is null.
   * managed by zms_output. */
  struct wl_list link;

  // TODO: replace with pixman
  struct zms_bgra* buffer;
  vec2 origin;
  int32_t width, height, stride;
};

struct zms_view* zms_view_create(struct zms_surface* surface);

void zms_view_destroy(struct zms_view* view);

int zms_view_commit(struct zms_view* view);

static inline bool
zms_view_is_mapped(struct zms_view* view)
{
  return view->priv->output != NULL;
}

static inline bool
zms_view_has_buffer(struct zms_view* view)
{
  return view->priv->buffer != NULL;
}

#endif  //  ZMONITORS_SERVER_VIEW_H
