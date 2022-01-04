#include "opengl-texture.h"

#include <zmonitors-util.h>

#include "backend.h"
#include "buffer.h"

ZMS_EXPORT struct zms_opengl_texture*
zms_opengl_texture_create_by_fd(
    struct zms_backend* backend, int fd, struct zms_screen_size size)
{
  struct zms_opengl_texture* texture;
  struct zgn_opengl_texture* proxy;
  struct zms_buffer* buffer;

  texture = zalloc(sizeof *texture);
  if (texture == NULL) goto err;

  proxy = zgn_opengl_create_texture(backend->opengl);
  if (proxy == NULL) goto err_proxy;

  buffer =
      zms_buffer_create_for_texture_by_fd(backend, fd, size.width, size.height);
  if (buffer == NULL) goto err_buffer;

  zgn_opengl_texture_attach_2d(proxy, buffer->proxy);

  texture->proxy = proxy;
  texture->buffer = buffer;

  return texture;

err_buffer:
  zgn_opengl_texture_destroy(proxy);

err_proxy:
  free(texture);

err:
  return NULL;
}

ZMS_EXPORT void
zms_opengl_texture_destroy(struct zms_opengl_texture* texture)
{
  zms_buffer_destroy(texture->buffer);
  zgn_opengl_texture_destroy(texture->proxy);
  free(texture);
}

ZMS_EXPORT void
zms_opengl_texture_buffer_updated(struct zms_opengl_texture* texture)
{
  zgn_opengl_texture_attach_2d(texture->proxy, texture->buffer->proxy);
}
