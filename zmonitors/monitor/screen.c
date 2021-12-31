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
ui_setup(struct zms_ui_base* ui_base)
{
  struct zms_screen* screen = ui_base->user_data;
  struct zms_backend* backend = screen->monitor->backend;
  struct zms_opengl_component* component;
  struct zms_opengl_shader_program* shader;
  struct zms_opengl_vertex_buffer* vertex_buffer;
  struct zms_opengl_texture* texture;
  int output_fd;

  component =
      zms_opengl_component_create(ui_base->root->cuboid_window->virtual_object);

  zms_opengl_component_set_topology(
      component, ZGN_OPENGL_TOPOLOGY_TRIANGLE_STRIP);
  zms_opengl_component_set_count(component, 4);

  shader = zms_opengl_shader_program_create(backend, screen_vertex_shader,
      sizeof(screen_vertex_shader), screen_fragment_shader,
      sizeof(screen_fragment_shader));

  vertex_buffer =
      zms_opengl_vertex_buffer_create(backend, sizeof(struct vertex_buffer));

  output_fd = zms_output_get_fd(screen->monitor->output);

  texture = zms_opengl_texture_create_by_fd(
      backend, output_fd, screen->monitor->screen_size);

  {
    int fd = zms_opengl_vertex_buffer_get_fd(vertex_buffer);
    struct vertex_buffer* data =
        mmap(NULL, sizeof(struct vertex_buffer), PROT_WRITE, MAP_SHARED, fd, 0);
    for (int i = 0; i < 4; i++) {
      glm_vec3_copy(ui_base->half_size, data->vertices[i].p);
      data->vertices[i].p[0] *= (i < 2 ? -1 : 1);
      data->vertices[i].p[1] *= (i % 2 == 1 ? 1 : -1);
      data->vertices[i].uv.u = (i < 2 ? 0 : 1);
      data->vertices[i].uv.v = (i % 2 == 1 ? 0 : 1);
    }
    munmap(data, sizeof(struct vertex_buffer));
  }

  zms_opengl_component_attach_vertex_buffer(component, vertex_buffer);
  zms_opengl_component_attach_shader_program(component, shader);
  zms_opengl_component_attach_texture(component, texture);

  zms_opengl_component_add_vertex_attribute(component, 0, 3,
      ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(struct vertex), 0);
  zms_opengl_component_add_vertex_attribute(component, 1, 2,
      ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(struct vertex),
      offsetof(struct vertex, uv));

  screen->component = component;
  screen->shader = shader;
  screen->vertex_buffer = vertex_buffer;
  screen->texture = texture;
}

static void
ui_teardown(struct zms_ui_base* ui_base)
{
  struct zms_screen* screen = ui_base->user_data;
  zms_opengl_texture_destroy(screen->texture);
  zms_opengl_vertex_buffer_destroy(screen->vertex_buffer);
  zms_opengl_shader_program_destroy(screen->shader);
  zms_opengl_component_destroy(screen->component);
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
