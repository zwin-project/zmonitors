#ifndef ZMONITORS_BACKEND_OPENGL_COMPONENT_H
#define ZMONITORS_BACKEND_OPENGL_COMPONENT_H

#include <zigen-opengl-client-protocol.h>

struct zms_opengl_component_private {
  struct zgn_opengl_component *proxy;
};

#endif  //  ZMONITORS_BACKEND_OPENGL_COMPONENT_H
