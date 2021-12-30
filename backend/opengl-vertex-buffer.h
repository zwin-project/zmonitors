#ifndef ZMONITORS_BACKEND_OPENGL_VERTEX_BUFFER_H
#define ZMONITORS_BACKEND_OPENGL_VERTEX_BUFFER_H

#include <zigen-opengl-client-protocol.h>

struct zms_opengl_vertex_buffer {
  struct zgn_opengl_vertex_buffer* proxy;
  struct zms_buffer* buffer;
};

#endif  //  ZMONITORS_BACKEND_OPENGL_VERTEX_BUFFER_H
