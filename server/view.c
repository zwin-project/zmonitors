#include "view.h"

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
  zms_output_remove_view(view->priv->output, view);
  free(view->priv);
  free(view);
}
