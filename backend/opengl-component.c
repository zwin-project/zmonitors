#include "opengl-component.h"

#include <zigen-opengl-client-protocol.h>
#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "backend.h"
#include "opengl-shader-program.h"
#include "opengl-texture.h"
#include "opengl-vertex-buffer.h"
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
  priv->texture = NULL;

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

ZMS_EXPORT void
zms_opengl_component_attach_vertex_buffer(
    struct zms_opengl_component* component,
    struct zms_opengl_vertex_buffer* vertex_buffer)
{
  zgn_opengl_component_attach_vertex_buffer(
      component->priv->proxy, vertex_buffer->proxy);
}

ZMS_EXPORT void
zms_opengl_component_attach_shader_program(
    struct zms_opengl_component* component,
    struct zms_opengl_shader_program* shader)
{
  zgn_opengl_component_attach_shader_program(
      component->priv->proxy, shader->proxy);
}

ZMS_EXPORT void
zms_opengl_component_attach_texture(
    struct zms_opengl_component* component, struct zms_opengl_texture* texture)
{
  zgn_opengl_component_attach_texture(component->priv->proxy, texture->proxy);
  component->priv->texture = texture;
}

ZMS_EXPORT void
zms_opengl_component_texture_updated(struct zms_opengl_component* component)
{
  if (component->priv->texture == NULL) return;
  zms_opengl_texture_buffer_updated(component->priv->texture);
  zgn_opengl_component_attach_texture(
      component->priv->proxy, component->priv->texture->proxy);
}

ZMS_EXPORT void
zms_opengl_component_set_min(
    struct zms_opengl_component* component, uint32_t min)
{
  zgn_opengl_component_set_min(component->priv->proxy, min);
}

ZMS_EXPORT void
zms_opengl_component_set_count(
    struct zms_opengl_component* component, uint32_t count)
{
  zgn_opengl_component_set_count(component->priv->proxy, count);
}

ZMS_EXPORT void
zms_opengl_component_set_topology(
    struct zms_opengl_component* component, uint32_t topology)
{
  zgn_opengl_component_set_topology(component->priv->proxy, topology);
}

ZMS_EXPORT void
zms_opengl_component_add_vertex_attribute(
    struct zms_opengl_component* component, uint32_t index, uint32_t size,
    uint32_t type, uint32_t normalized, uint32_t stride, uint32_t pointer)
{
  zgn_opengl_component_add_vertex_attribute(
      component->priv->proxy, index, size, type, normalized, stride, pointer);
}
