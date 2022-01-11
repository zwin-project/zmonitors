#ifndef ZMONITORS_PIXMAN_HELPER_H
#define ZMONITORS_PIXMAN_HELPER_H

#include <pixman-1/pixman.h>

#include "view.h"

static inline void
pixman_region32_init_view_global(
    pixman_region32_t *region, struct zms_view *view)
{
  pixman_region32_init_rect(region, view->priv->origin[0],
      view->priv->origin[1], zms_view_get_width(view),
      zms_view_get_height(view));
}

static inline void
pixman_transform_init_view_global(
    pixman_transform_t *transform, struct zms_view *view)
{
  pixman_transform_init_translate(transform,
      pixman_double_to_fixed(-view->priv->origin[0]),
      pixman_double_to_fixed(-view->priv->origin[1]));
}

#endif  //  ZMONITORS_PIXMAN_HELPER_H
