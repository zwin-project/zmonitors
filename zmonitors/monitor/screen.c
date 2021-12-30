#include "screen.h"

#include <zigen-opengl-client-protocol.h>
#include <zmonitors-util.h>

#include "monitor-internal.h"
#include "screen-frag.h"
#include "screen-vert.h"
#include "sys/mman.h"

struct uv {
  float u, v;
};

struct vertex {
  vec3 p;
  struct uv uv;
};

struct vertex_buffer {
  struct vertex vertices[4];
};

static void
ui_setup(void* data)
{
  struct zms_screen* screen = data;
  struct zms_backend* backend = screen->monitor->backend;
  struct zms_opengl_component* component;
  struct zms_opengl_shader_program* shader;
  struct zms_opengl_vertex_buffer* vertex_buffer;

  component = zms_ui_base_get_component(screen->base);

  // TODO: move these api calls to ui_base

  zms_opengl_component_set_topology(
      component, ZGN_OPENGL_TOPOLOGY_TRIANGLE_STRIP);
  zms_opengl_component_set_count(component, 4);

  shader = zms_opengl_shader_program_create(backend, screen_vertex_shader,
      sizeof(screen_vertex_shader), screen_fragment_shader,
      sizeof(screen_fragment_shader));

  vertex_buffer =
      zms_opengl_vertex_buffer_create(backend, sizeof(struct vertex_buffer));

  {
    // FIXME:
    int fd = zms_opengl_vertex_buffer_get_fd(vertex_buffer);
    struct vertex_buffer* data =
        mmap(NULL, sizeof(struct vertex_buffer), PROT_WRITE, MAP_SHARED, fd, 0);
    vec3 A = {-0.2f, -0.2f, 0.0f};
    vec3 B = {-0.2f, +0.2f, 0.0f};
    vec3 C = {+0.2f, -0.2f, 0.0f};
    vec3 D = {+0.2f, +0.2f, 0.0f};
    glm_vec3_copy(A, data->vertices[0].p);
    glm_vec3_copy(B, data->vertices[1].p);
    glm_vec3_copy(C, data->vertices[2].p);
    glm_vec3_copy(D, data->vertices[3].p);
    munmap(data, sizeof(struct vertex_buffer));
  }

  zms_opengl_component_attach_vertex_buffer(component, vertex_buffer);
  zms_opengl_component_attach_shader_program(component, shader);

  zms_opengl_component_add_vertex_attribute(component, 0, 3,
      ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(struct vertex), 0);

  screen->shader = shader;
  screen->vertex_buffer = vertex_buffer;
}

static void
ui_teardown(void* data)
{
  struct zms_screen* screen = data;
  zms_opengl_vertex_buffer_destroy(screen->vertex_buffer);
  zms_opengl_shader_program_destroy(screen->shader);
  screen->shader = NULL;
}

static const struct zms_ui_base_interface ui_base_interface = {
    .setup = ui_setup,
    .teardown = ui_teardown,
};

ZMS_EXPORT struct zms_screen*
zms_screen_create(struct zms_monitor* monitor)
{
  struct zms_screen* screen;
  struct zms_ui_base* base;
  struct zms_ui_base* parent = zms_ui_root_get_base(monitor->ui_root);

  screen = zalloc(sizeof *screen);
  if (screen == NULL) goto err;

  base = zms_ui_base_create(screen, &ui_base_interface, parent);
  if (base == NULL) goto err_base;

  screen->base = base;
  screen->monitor = monitor;
  screen->shader = NULL;

  return screen;

err_base:
  free(screen);

err:
  return NULL;
}

ZMS_EXPORT void
zms_screen_destroy(struct zms_screen* screen)
{
  zms_ui_base_destroy(screen->base);
  free(screen);
}
