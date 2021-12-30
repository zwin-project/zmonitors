#ifndef ZMONITORS_BACKEND_OPENGL_SHADER_PROGRAM_H
#define ZMONITORS_BACKEND_OPENGL_SHADER_PROGRAM_H

#include <zigen-opengl-client-protocol.h>
#include <zmonitors-backend.h>

struct zms_opengl_shader_program {
  struct zgn_opengl_shader_program* proxy;
  int vertex_shader_fd;
  int fragment_shader_fd;
};

#endif  //  ZMONITORS_BACKEND_OPENGL_SHADER_PROGRAM_H
