#include "view-layer.h"

#include <zmonitors-util.h>

ZMS_EXPORT void
zms_view_layer_init(struct zms_view_layer* view_layer)
{
  wl_list_init(&view_layer->view_list);
}
