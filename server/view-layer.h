#ifndef ZMONITORS_VIEW_LAYER_H
#define ZMONITORS_VIEW_LAYER_H

#include <zmonitors-util.h>

struct zms_view_layer {
  struct wl_list view_list;
};

void zms_view_layer_init(struct zms_view_layer* view_layer);

#define zms_view_layer_for_each(view, view_layer) \
  wl_list_for_each(view, &(view_layer)->view_list, link)

#endif  //  ZMONITORS_VIEW_LAYER_H
