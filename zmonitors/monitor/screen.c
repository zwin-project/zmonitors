#include "screen.h"

#include <sys/mman.h>
#include <zigen-opengl-client-protocol.h>
#include <zmonitors-util.h>

#include "data-source-proxy.h"
#include "intersect.h"
#include "monitor-internal.h"
#include "screen-frag.h"
#include "screen-vert.h"

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
zms_screen_calculate_corner_points(struct zms_screen* screen)
{
  struct zms_ui_base* ui_base = screen->base;
  struct zms_cuboid_window* cuboid_window = ui_base->root->cuboid_window;
  vec3 left_top;
  glm_vec3_copy(ui_base->half_size, left_top);
  left_top[0] *= -1;

  glm_vec3_add(ui_base->position, left_top, screen->v0);
  glm_quat_rotatev(cuboid_window->quaternion, screen->v0, screen->v0);

  glm_vec3_add(ui_base->position, ui_base->half_size, screen->vx);
  glm_quat_rotatev(cuboid_window->quaternion, screen->vx, screen->vx);

  glm_vec3_sub(ui_base->position, ui_base->half_size, screen->vy);
  glm_quat_rotatev(cuboid_window->quaternion, screen->vy, screen->vy);
}

static bool
ray_enter(
    struct zms_ui_base* ui_base, uint32_t serial, vec3 origin, vec3 direction)
{
  Z_UNUSED(serial);
  struct zms_screen* screen = ui_base->user_data;
  vec2 pos;
  float d;

  if (zms_interesect_ray_rect(
          origin, direction, screen->v0, screen->vx, screen->vy, pos, &d)) {
    screen->ray_focus = true;
    pos[0] *= screen->monitor->screen_size.width;
    pos[1] *= screen->monitor->screen_size.height;
    zms_seat_notify_pointer_enter(
        screen->monitor->compositor->seat, screen->output, pos);
  }

  return true;
}

static bool
ray_motion(
    struct zms_ui_base* ui_base, uint32_t time, vec3 origin, vec3 direction)
{
  struct zms_screen* screen = ui_base->user_data;
  vec2 pos;
  float d;

  if (zms_interesect_ray_rect(
          origin, direction, screen->v0, screen->vx, screen->vy, pos, &d)) {
    pos[0] *= screen->monitor->screen_size.width;
    pos[1] *= screen->monitor->screen_size.height;
    if (screen->ray_focus) {
      zms_seat_notify_pointer_motion_abs(
          screen->monitor->compositor->seat, screen->output, pos, time);
    } else {
      zms_seat_notify_pointer_enter(
          screen->monitor->compositor->seat, screen->output, pos);
      screen->ray_focus = true;
    }
  } else {
    if (screen->ray_focus)
      zms_seat_notify_pointer_leave(screen->monitor->compositor->seat);
    screen->ray_focus = false;
  }

  return true;
}

static bool
ray_leave(struct zms_ui_base* ui_base, uint32_t serial)
{
  Z_UNUSED(serial);
  struct zms_screen* screen = ui_base->user_data;

  if (screen->ray_focus)
    zms_seat_notify_pointer_leave(screen->monitor->compositor->seat);

  screen->ray_focus = false;

  return true;
}

static bool
ray_button(struct zms_ui_base* ui_base, uint32_t serial, uint32_t time,
    uint32_t button, uint32_t state)
{
  struct zms_screen* screen = ui_base->user_data;

  if (screen->ray_focus) {
    zms_seat_notify_pointer_button(
        screen->monitor->compositor->seat, time, button, state, serial);
  }

  return true;
}

static bool
data_device_enter(struct zms_ui_base* ui_base, uint32_t serial, vec3 origin,
    vec3 direction, struct zms_data_offer_proxy* data_offer_proxy)
{
  Z_UNUSED(data_offer_proxy);
  struct zms_screen* screen = ui_base->user_data;
  vec2 pos;
  float d;

  screen->data_device_enter_serial = serial;

  if (zms_interesect_ray_rect(
          origin, direction, screen->v0, screen->vx, screen->vy, pos, &d)) {
    pos[0] *= screen->monitor->screen_size.width;
    pos[1] *= screen->monitor->screen_size.height;
    zms_seat_notify_start_drag(screen->monitor->compositor->seat, serial);
    zms_seat_notify_pointer_enter(
        screen->monitor->compositor->seat, screen->output, pos);
    screen->data_device_focus = true;
  }

  return true;
}

static bool
keyboard_enter(
    struct zms_ui_base* ui_base, uint32_t serial, struct wl_array* keys)
{
  Z_UNUSED(serial);
  Z_UNUSED(keys);
  struct zms_screen* screen = ui_base->user_data;

  zms_seat_notify_keyboard_enter(screen->monitor->compositor->seat);

  return true;
}

static bool
data_device_leave(struct zms_ui_base* ui_base)
{
  struct zms_screen* screen = ui_base->user_data;

  zms_data_device_notify_leave(screen->monitor->compositor->seat->data_device);

  if (screen->data_device_focus)
    zms_seat_notify_pointer_leave(screen->monitor->compositor->seat);
  else
    zms_data_device_destroy_data_offer(
        screen->monitor->compositor->seat->data_device);

  screen->data_device_focus = false;

  return true;
}

static bool
keyboard_leave(struct zms_ui_base* ui_base, uint32_t serial)
{
  Z_UNUSED(serial);
  struct zms_screen* screen = ui_base->user_data;

  zms_seat_notify_keyboard_leave(screen->monitor->compositor->seat);

  return true;
}

static bool
data_device_motion_abs(
    struct zms_ui_base* ui_base, uint32_t time, vec3 origin, vec3 direction)
{
  struct zms_screen* screen = ui_base->user_data;
  vec2 pos;
  float d;

  if (zms_interesect_ray_rect(
          origin, direction, screen->v0, screen->vx, screen->vy, pos, &d)) {
    pos[0] *= screen->monitor->screen_size.width;
    pos[1] *= screen->monitor->screen_size.height;
    if (screen->data_device_focus) {
      zms_seat_notify_pointer_motion_abs(
          screen->monitor->compositor->seat, screen->output, pos, time);
    } else {
      zms_seat_notify_start_drag(
          screen->monitor->compositor->seat, screen->data_device_enter_serial);
      zms_seat_notify_pointer_enter(
          screen->monitor->compositor->seat, screen->output, pos);
      screen->data_device_focus = true;
    }
  } else {
    if (screen->data_device_focus)
      zms_seat_notify_pointer_leave(screen->monitor->compositor->seat);
    screen->data_device_focus = false;
  }

  return true;
}

static bool
keyboard_key(struct zms_ui_base* ui_base, uint32_t serial, uint32_t time,
    uint32_t key, uint32_t state)
{
  struct zms_screen* screen = ui_base->user_data;

  zms_seat_notify_keyboard_key(
      screen->monitor->compositor->seat, serial, time, key, state);

  return true;
}

static bool
data_device_drop(struct zms_ui_base* ui_base)
{
  struct zms_screen* screen = ui_base->user_data;

  zms_data_device_notify_drop(screen->monitor->compositor->seat->data_device);

  return true;
}

static bool
keyboard_modifiers(struct zms_ui_base* ui_base, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
    uint32_t group)
{
  struct zms_screen* screen = ui_base->user_data;

  zms_seat_notify_keyboard_modifiers(screen->monitor->compositor->seat, serial,
      mods_depressed, mods_latched, mods_locked, group);

  return true;
}

static void
ui_setup(struct zms_ui_base* ui_base)
{
  struct zms_screen* screen = ui_base->user_data;
  struct zms_screen_size screen_size = screen->monitor->screen_size;
  struct zms_backend* backend = screen->monitor->backend;
  struct zms_opengl_component* component;
  struct zms_opengl_shader_program* shader;
  struct zms_opengl_vertex_buffer* vertex_buffer;
  struct zms_pixel_buffer* pixel_buffer;
  mat4 transform;

  zms_screen_calculate_corner_points(screen);

  glm_quat_mat4(ui_base->root->cuboid_window->quaternion, transform);
  glm_translate(transform, ui_base->position);

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

  for (int i = 0; i < screen->output->pixel_buffer_count; i++) {
    struct zms_pixel_buffer* pb = screen->output->pixel_buffers[i];
    screen->textures[i] =
        zms_opengl_texture_create_by_fd(backend, pb->fd, screen_size);
    pb->user_data = screen->textures[i];
  }

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

  zms_opengl_shader_program_set_uniform_variable_mat4(
      shader, "transform", transform);

  zms_opengl_component_attach_vertex_buffer(component, vertex_buffer);
  zms_opengl_component_attach_shader_program(component, shader);
  pixel_buffer = zms_output_buffer_ring_rotate(screen->output);
  zms_opengl_component_attach_texture(component, pixel_buffer->user_data);

  zms_opengl_component_add_vertex_attribute(component, 0, 3,
      ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(struct vertex), 0);
  zms_opengl_component_add_vertex_attribute(component, 1, 2,
      ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(struct vertex),
      offsetof(struct vertex, uv));

  screen->component = component;
  screen->shader = shader;
  screen->vertex_buffer = vertex_buffer;
}

static void
ui_teardown(struct zms_ui_base* ui_base)
{
  struct zms_screen* screen = ui_base->user_data;

  for (int i = 0; i < screen->output->pixel_buffer_count; i++)
    zms_opengl_texture_destroy(screen->textures[i]);

  zms_opengl_vertex_buffer_destroy(screen->vertex_buffer);
  zms_opengl_shader_program_destroy(screen->shader);
  zms_opengl_component_destroy(screen->component);
}

static void
ui_reconfigure(struct zms_ui_base* ui_base)
{
  mat4 transform;
  struct zms_screen* screen = ui_base->user_data;

  zms_screen_calculate_corner_points(screen);

  glm_quat_mat4(ui_base->root->cuboid_window->quaternion, transform);
  glm_translate(transform, ui_base->position);

  zms_opengl_shader_program_set_uniform_variable_mat4(
      screen->shader, "transform", transform);
  zms_opengl_component_attach_shader_program(screen->component, screen->shader);
  zms_ui_base_schedule_repaint(ui_base);
}

static void
ui_repaint(struct zms_ui_base* ui_base)
{
  struct zms_screen* screen = ui_base->user_data;
  struct zms_pixel_buffer* pixel_buffer;
  struct zms_opengl_texture* texture;

  if (screen->texture_changed) {
    pixel_buffer = zms_output_buffer_ring_rotate(screen->output);
    texture = pixel_buffer->user_data;

    zms_opengl_component_attach_texture(screen->component, texture);
    zms_opengl_component_texture_updated(screen->component);
    screen->texture_changed = false;
  }
}

static void
ui_frame(struct zms_ui_base* ui_base, uint32_t time)
{
  struct zms_screen* screen = ui_base->user_data;
  zms_output_frame(screen->output, time);
}

static const struct zms_ui_base_interface ui_base_interface = {
    .setup = ui_setup,
    .teardown = ui_teardown,
    .reconfigure = ui_reconfigure,
    .repaint = ui_repaint,
    .frame = ui_frame,
    .ray_enter = ray_enter,
    .ray_leave = ray_leave,
    .ray_motion = ray_motion,
    .ray_button = ray_button,
    .data_device_enter = data_device_enter,
    .data_device_leave = data_device_leave,
    .data_device_motion_abs = data_device_motion_abs,
    .data_device_drop = data_device_drop,
    .keyboard_enter = keyboard_enter,
    .keyboard_leave = keyboard_leave,
    .keyboard_key = keyboard_key,
    .keyboard_modifiers = keyboard_modifiers,
};

static void
schedule_output_repainting(void* data, struct zms_output* output)
{
  Z_UNUSED(output);
  struct zms_screen* screen = data;
  screen->texture_changed = true;
  zms_ui_base_schedule_repaint(screen->base);
}

static void
start_drag(
    void* data, struct zms_data_source_proxy* proxy_base, uint32_t serial)
{
  struct zms_screen* screen = data;
  struct zms_app_data_source_proxy* proxy;
  struct zms_virtual_object* virtual_object;

  proxy = wl_container_of(proxy_base, proxy, base);
  virtual_object = screen->base->root->cuboid_window->virtual_object;

  zms_backend_data_device_start_drag(screen->monitor->backend,
      proxy->backend_data_source, virtual_object, NULL, serial);
}

static const struct zms_output_interface output_interface = {
    .schedule_repaint = schedule_output_repainting,
    .start_drag = start_drag,
};

ZMS_EXPORT struct zms_screen*
zms_screen_create(struct zms_monitor* monitor)
{
  struct zms_screen* screen;
  struct zms_ui_base* base;
  struct zms_output* output;
  struct zms_ui_base* parent = monitor->ui_root->base;
  struct zms_opengl_texture** textures;
  vec2 physical_size;

  screen = zalloc(sizeof *screen);
  if (screen == NULL) goto err;

  base = zms_ui_base_create(screen, &ui_base_interface, parent);
  if (base == NULL) goto err_base;

  physical_size[0] = (float)monitor->screen_size.width / 2 / monitor->ppm;
  physical_size[1] = (float)monitor->screen_size.height / 2 / monitor->ppm;
  output = zms_output_create(monitor->compositor, monitor->screen_size,
      physical_size, "zmonitors", "virtual monitor");
  if (output == NULL) goto err_output;
  zms_output_set_implementation(output, screen, &output_interface);

  textures = zalloc(sizeof(*textures) * output->pixel_buffer_count);
  if (textures == NULL) goto err_textures;

  screen->base = base;
  screen->monitor = monitor;
  screen->output = output;
  screen->textures = textures;

  screen->texture_changed = false;
  screen->ray_focus = false;
  screen->data_device_focus = false;
  screen->data_device_enter_serial = 0;

  return screen;

err_textures:
  zms_output_destroy(output);

err_output:
  zms_ui_base_destroy(base);

err_base:
  free(screen);

err:
  return NULL;
}

ZMS_EXPORT void
zms_screen_destroy(struct zms_screen* screen)
{
  zms_output_destroy(screen->output);
  zms_ui_base_destroy(screen->base);
  free(screen->textures);
  free(screen);
}
