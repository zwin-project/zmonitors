#ifndef ZMONITORS_BACKEND_OPENGL_TEXTURE_H
#define ZMONITORS_BACKEND_OPENGL_TEXTURE_H

#include <zigen-opengl-client-protocol.h>

struct zms_opengl_texture {
  struct zgn_opengl_texture *proxy;
  struct zms_buffer *buffer;
};

#endif  //  ZMONITORS_BACKEND_OPENGL_TEXTURE_H
