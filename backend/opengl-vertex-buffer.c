#include "opengl-vertex-buffer.h"

#include <zmonitors-backend.h>

#include "backend.h"
#include "buffer.h"

ZMS_EXPORT struct zms_opengl_vertex_buffer*
zms_opengl_vertex_buffer_create(struct zms_backend* backend, size_t size)
{
  struct zms_opengl_vertex_buffer* vertex_buffer;
  struct zgn_opengl_vertex_buffer* proxy;
  struct zms_buffer* buffer;

  vertex_buffer = zalloc(sizeof *vertex_buffer);
  if (vertex_buffer == NULL) goto err;

  proxy = zgn_opengl_create_vertex_buffer(backend->opengl);
  if (proxy == NULL) goto err_proxy;

  buffer = zms_buffer_create(backend, size);
  if (buffer == NULL) goto err_buffer;

  zgn_opengl_vertex_buffer_attach(proxy, buffer->proxy);

  vertex_buffer->proxy = proxy;
  vertex_buffer->buffer = buffer;

  return vertex_buffer;

err_buffer:
  zgn_opengl_vertex_buffer_destroy(proxy);

err_proxy:
  free(vertex_buffer);

err:
  return NULL;
}

ZMS_EXPORT void
zms_opengl_vertex_buffer_destroy(struct zms_opengl_vertex_buffer* vertex_buffer)
{
  zms_buffer_destroy(vertex_buffer->buffer);
  zgn_opengl_vertex_buffer_destroy(vertex_buffer->proxy);
  free(vertex_buffer);
}

ZMS_EXPORT int
zms_opengl_vertex_buffer_get_fd(struct zms_opengl_vertex_buffer* vertex_buffer)
{
  return vertex_buffer->buffer->fd;
}
