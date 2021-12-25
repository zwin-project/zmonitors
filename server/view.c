#include "view.h"

#include <zmonitors.h>

#include "compositor.h"

ZMS_EXPORT struct zms_view*
zms_view_create(struct zms_compositor* compositor)
{
  struct zms_view* view;
  struct zms_view_private* priv;

  view = zalloc(sizeof *view);
  if (view == NULL) goto err;

  priv = zalloc(sizeof *priv);
  if (priv == NULL) goto err_priv;

  priv->pub = view;

  view->priv = priv;
  zms_list_insert(&compositor->view_list, &view->link);

  return view;

err_priv:
  free(view);

err:
  return NULL;
}

ZMS_EXPORT void
zms_view_destroy(struct zms_view* view)
{
  zms_list_remove(&view->link);
  free(view->priv);
  free(view);
}
