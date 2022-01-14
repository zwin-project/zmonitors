#ifndef ZMONITORS_BACKEND_OPENGL_COMPONENT_H
#define ZMONITORS_BACKEND_OPENGL_COMPONENT_H

#include <zigen-opengl-client-protocol.h>
#include <zmonitors-backend.h>

struct zms_opengl_component_private {
  struct zgn_opengl_component* proxy;
  struct zms_opengl_texture* texture; /* nullable */
};

#endif  //  ZMONITORS_BACKEND_OPENGL_COMPONENT_H
