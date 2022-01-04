#include "view.h"

#include <zmonitors-server.h>

#include "compositor.h"
#include "output.h"

ZMS_EXPORT struct zms_view*
zms_view_create(struct zms_compositor* compositor)
{
  struct zms_view* view;
  struct zms_view_private* priv;
  struct zms_output* primary_output;

  view = zalloc(sizeof *view);
  if (view == NULL) goto err;

  priv = zalloc(sizeof *priv);
  if (priv == NULL) goto err_priv;

  priv->pub = view;
  priv->output = NULL;

  view->priv = priv;

  primary_output = zms_compositor_get_primary_output(compositor);
  zms_output_add_view(primary_output, view);

  return view;

err_priv:
  free(view);

err:
  return NULL;
}

ZMS_EXPORT void
zms_view_destroy(struct zms_view* view)
{
  zms_output_remove_view(view);
  free(view->priv);
  free(view);
}
