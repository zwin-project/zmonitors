#include "opengl-shader-program.h"

#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <zmonitors-backend.h>
#include <zmonitors-util.h>

#include "backend.h"

static int
create_shared_fd(off_t size)
{
  const char* name = "zms-shader";
  int fd = memfd_create(name, MFD_CLOEXEC | MFD_ALLOW_SEALING);
  if (fd < 0) return fd;
  unlink(name);

  if (ftruncate(fd, size) < 0) {
    close(fd);
    return -1;
  }

  return fd;
}

ZMS_EXPORT struct zms_opengl_shader_program*
zms_opengl_shader_program_create(struct zms_backend* backend,
    const char* vertex_shader, size_t vertex_shader_size,
    const char* fragment_shader, size_t fragment_shader_size)
{
  struct zms_opengl_shader_program* program;
  struct zgn_opengl_shader_program* proxy;
  int vertex_shader_fd;
  int fragment_shader_fd;

  program = zalloc(sizeof *program);
  if (program == NULL) goto err;

  proxy = zgn_opengl_create_shader_program(backend->opengl);
  if (proxy == NULL) goto err_proxy;

  vertex_shader_fd = create_shared_fd(vertex_shader_size);
  {
    void* data = mmap(
        NULL, vertex_shader_size, PROT_WRITE, MAP_SHARED, vertex_shader_fd, 0);
    if (data == MAP_FAILED) goto err_vertex_shader;
    memcpy(data, vertex_shader, vertex_shader_size);
    munmap(data, vertex_shader_size);
  }

  zgn_opengl_shader_program_set_vertex_shader(
      proxy, vertex_shader_fd, vertex_shader_size);

  fragment_shader_fd = create_shared_fd(fragment_shader_size);
  {
    void* data = mmap(NULL, fragment_shader_size, PROT_WRITE, MAP_SHARED,
        fragment_shader_fd, 0);
    if (data == MAP_FAILED) goto err_fragment_shader;
    memcpy(data, fragment_shader, fragment_shader_size);
    munmap(data, fragment_shader_size);
  }

  zgn_opengl_shader_program_set_fragment_shader(
      proxy, fragment_shader_fd, fragment_shader_size);

  zgn_opengl_shader_program_link(proxy);

  program->proxy = proxy;
  program->vertex_shader_fd = vertex_shader_fd;
  program->fragment_shader_fd = fragment_shader_fd;

  return program;

err_fragment_shader:
  close(fragment_shader_fd);

err_vertex_shader:
  close(vertex_shader_fd);

err_proxy:
  free(program);

err:
  return NULL;
}

ZMS_EXPORT void
zms_opengl_shader_program_destroy(struct zms_opengl_shader_program* program)
{
  close(program->vertex_shader_fd);
  close(program->fragment_shader_fd);
  zgn_opengl_shader_program_destroy(program->proxy);
  free(program);
}
