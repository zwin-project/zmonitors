#include "opengl-component.h"

#include <zigen-opengl-client-protocol.h>
#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "backend.h"
#include "virtual-object.h"

ZMS_EXPORT struct zms_opengl_component*
zms_opengl_component_create(struct zms_virtual_object* virtual_object)
{
  struct zms_opengl_component* component;
  struct zms_opengl_component_private* priv;
  struct zgn_opengl_component* proxy;
  struct zms_backend* backend = virtual_object->backend;

  component = zalloc(sizeof *component);
  if (component == NULL) goto err;

  priv = zalloc(sizeof *priv);
  if (priv == NULL) goto err_priv;

  proxy = zgn_opengl_create_opengl_component(
      backend->opengl, virtual_object->proxy);
  if (proxy == NULL) goto err_proxy;

  priv->proxy = proxy;
  wl_proxy_set_user_data((struct wl_proxy*)proxy, component);

  component->priv = priv;

  return component;

err_proxy:
  free(priv);

err_priv:
  free(component);

err:
  return NULL;
}

ZMS_EXPORT void
zms_opengl_component_destroy(struct zms_opengl_component* component)
{
  zgn_opengl_component_destroy(component->priv->proxy);
  free(component->priv);
  free(component);
}
