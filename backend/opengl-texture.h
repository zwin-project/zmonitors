#ifndef ZMONITORS_BACKEND_OPENGL_TEXTURE_H
#define ZMONITORS_BACKEND_OPENGL_TEXTURE_H

#include <zigen-opengl-client-protocol.h>

struct zms_opengl_texture {
  struct zgn_opengl_texture *proxy;
  struct zms_buffer *buffer;
};

void zms_opengl_texture_buffer_updated(struct zms_opengl_texture *texture);

#endif  //  ZMONITORS_BACKEND_OPENGL_TEXTURE_H
