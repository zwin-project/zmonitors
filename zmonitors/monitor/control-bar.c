#include "control-bar.h"

#include <string.h>
#include <sys/mman.h>
#include <zigen-opengl-client-protocol.h>
#include <zmonitors-util.h>

#include "control-bar-frag.h"
#include "control-bar-vert.h"
#include "monitor-internal.h"

struct uv {
  float u, v;
};

struct vertex {
  vec3 p;
  struct uv uv;
};

struct vertex_buffer {
  struct vertex vertices[8];
};

static void
ui_setup(struct zms_ui_base *ui_base)
{
  struct zms_control_bar *control_bar = ui_base->user_data;
  struct zms_backend *backend = control_bar->monitor->backend;
  struct zms_opengl_component *component;
  struct zms_opengl_shader_program *shader;
  struct zms_opengl_vertex_buffer *vertex_buffer;
  struct zms_opengl_texture *texture;
  struct zms_screen_size texture_size = {200, 100};
  mat4 transform;

  glm_translate_make(transform, ui_base->position);

  component =
      zms_opengl_component_create(ui_base->root->cuboid_window->virtual_object);

  zms_opengl_component_set_topology(
      component, ZGN_OPENGL_TOPOLOGY_TRIANGLE_STRIP);
  zms_opengl_component_set_count(
      component, sizeof(struct vertex_buffer) / sizeof(struct vertex));

  shader = zms_opengl_shader_program_create(backend, control_bar_vertex_shader,
      sizeof(control_bar_vertex_shader), control_bar_fragment_shader,
      sizeof(control_bar_fragment_shader));

  vertex_buffer =
      zms_opengl_vertex_buffer_create(backend, sizeof(struct vertex_buffer));

  texture = zms_opengl_texture_create(backend, texture_size);

  {
    float w = ui_base->half_size[0];
    float h = ui_base->half_size[1];
    // clang-format off
    struct vertex buffer[8] = {
        {{-w,     h, 0}, {0.00, 1}}, {{-w    , -h, 0}, {0.00, 0}},
        {{-w + h, h, 0}, {0.25, 1}}, {{-w + h, -h, 0}, {0.25, 0}},
        {{ w - h, h, 0}, {0.75, 1}}, {{ w - h, -h, 0}, {0.75, 0}},
        {{ w    , h, 0}, {1.00, 1}}, {{ w    , -h, 0}, {1.00, 0}},
    };
    // clang-format on
    int fd = zms_opengl_vertex_buffer_get_fd(vertex_buffer);
    struct vertex_buffer *data =
        mmap(NULL, sizeof(struct vertex_buffer), PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(data, buffer, sizeof(struct vertex_buffer));
    munmap(data, sizeof(struct vertex_buffer));
  }

  {
    int fd = zms_opengl_texture_get_fd(texture);
    size_t data_size =
        sizeof(struct zms_bgra) * texture_size.width * texture_size.height;

    struct zms_bgra *data =
        mmap(NULL, data_size, PROT_WRITE, MAP_SHARED, fd, 0);

    size_t i = 0;
    int32_t width_1_4 = texture_size.width / 4;
    int32_t width_3_4 = texture_size.width * 3 / 4;
    int32_t height_1_2 = texture_size.height / 2;
    int32_t height_1_2_2 = height_1_2 * height_1_2;
    for (int32_t y = 0; y < texture_size.height; y++) {
      int32_t dy = height_1_2 - y;
      int32_t dy2 = dy * dy;
      for (int32_t x = 0; x < texture_size.width; x++) {
        data[i].b = 0xff;
        data[i].g = 0xff;
        data[i].r = 0xff;
        if (width_1_4 <= x && x <= width_3_4) {
          data[i].a = 0xff;
        } else {
          int32_t dx;
          if (x < width_1_4) {
            dx = width_1_4 - x;
          } else {
            dx = x - width_3_4;
          }
          if (dx * dx + dy2 < height_1_2_2) {
            data[i].a = 0xff;
          } else {
            data[i].a = 0x00;
          }
        }
        i++;
      }
    }
    munmap(data, data_size);
  }

  zms_opengl_shader_program_set_uniform_variable_mat4(
      shader, "transform", transform);

  zms_opengl_component_attach_vertex_buffer(component, vertex_buffer);
  zms_opengl_component_attach_shader_program(component, shader);
  zms_opengl_component_attach_texture(component, texture);

  zms_opengl_component_add_vertex_attribute(component, 0, 3,
      ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(struct vertex), 0);
  zms_opengl_component_add_vertex_attribute(component, 1, 2,
      ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(struct vertex),
      offsetof(struct vertex, uv));

  control_bar->component = component;
  control_bar->shader = shader;
  control_bar->vertex_buffer = vertex_buffer;
  control_bar->texture = texture;
}

static void
ui_teardown(struct zms_ui_base *ui_base)
{
  struct zms_control_bar *control_bar = ui_base->user_data;
  zms_opengl_texture_destroy(control_bar->texture);
  zms_opengl_vertex_buffer_destroy(control_bar->vertex_buffer);
  zms_opengl_shader_program_destroy(control_bar->shader);
  zms_opengl_component_destroy(control_bar->component);
}

static const struct zms_ui_base_interface ui_base_interface = {
    .setup = ui_setup,
    .teardown = ui_teardown,
};

ZMS_EXPORT struct zms_control_bar *
zms_control_bar_create(struct zms_monitor *monitor)
{
  struct zms_control_bar *control_bar;
  struct zms_ui_base *base;
  struct zms_ui_base *parent = monitor->ui_root->base;

  control_bar = zalloc(sizeof *control_bar);
  if (control_bar == NULL) goto err;

  base = zms_ui_base_create(control_bar, &ui_base_interface, parent);
  if (base == NULL) goto err_base;

  control_bar->base = base;
  control_bar->monitor = monitor;

  return control_bar;

err_base:
  free(control_bar);

err:
  return NULL;
}

ZMS_EXPORT void
zms_control_bar_destroy(struct zms_control_bar *control_bar)
{
  zms_ui_base_destroy(control_bar->base);
  free(control_bar);
}
