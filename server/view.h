#ifndef ZMONITORS_SERVER_VIEW_H
#define ZMONITORS_SERVER_VIEW_H

#include <wayland-server.h>
#include <zmonitors-server.h>

#include "surface.h"

struct zms_view_private {
  struct zms_view* pub;

  /* be careful of its data consistency with zms_view_private.link; nullable */
  struct zms_output* output;
  struct wl_list link; /* must be in the self->output->priv->view_list */
};

struct zms_view* zms_view_create(struct zms_compositor* compositor);

void zms_view_destroy(struct zms_view* view);

#endif  //  ZMONITORS_SERVER_VIEW_H
