#ifndef ZMONITORS_BACKEND_H
#define ZMONITORS_BACKEND_H

#include <cglm/cglm.h>
#include <stdbool.h>
#include <zmonitors-util.h>

/* backend */

struct zms_backend;

struct zms_backend_interface {
  void (*gain_ray_capability)(void* data);
  void (*lose_ray_capability)(void* data);
};

struct zms_backend* zms_backend_create(
    void* user_data, const struct zms_backend_interface* interface);

void zms_backend_destroy(struct zms_backend* backend);

bool zms_backend_connect(struct zms_backend* backend, const char* socket);

int zms_backend_get_fd(struct zms_backend* backend);

int zms_backend_dispatch(struct zms_backend* backend);

int zms_backend_flush(struct zms_backend* backend);

int zms_backend_dispatch_pending(struct zms_backend* backend);

/* virtual object */

struct zms_virtual_object;

/* cuboid window */

struct zms_cuboid_window_private;

struct zms_cuboid_window {
  struct zms_cuboid_window_private* priv;
  struct zms_backend* backend;
  struct zms_virtual_object* virtual_object;

  vec3 half_size;
  versor quaternion;

  void (*configured)(void* user_data, struct zms_cuboid_window* cuboid_window);
};

struct zms_cuboid_window* zms_cuboid_window_create(void* user_data,
    struct zms_backend* backend, vec3 half_size, versor quaternion);

void zms_cuboid_window_destroy(struct zms_cuboid_window* cuboid_window);

void zms_cuboid_window_commit(struct zms_cuboid_window* cuboid_window);

/* opengl shader */

struct zms_opengl_shader_program;

struct zms_opengl_shader_program* zms_opengl_shader_program_create(
    struct zms_backend* backend, const char* vertex_shader,
    size_t vertex_shader_size, const char* fragment_shader,
    size_t fragment_shader_size);

void zms_opengl_shader_program_destroy(
    struct zms_opengl_shader_program* program);

/* opengl vertex buffer*/

struct zms_opengl_vertex_buffer;

struct zms_opengl_vertex_buffer* zms_opengl_vertex_buffer_create(
    struct zms_backend* backend, size_t size);

void zms_opengl_vertex_buffer_destroy(
    struct zms_opengl_vertex_buffer* vertex_buffer);

int zms_opengl_vertex_buffer_get_fd(
    struct zms_opengl_vertex_buffer* vertex_buffer);

/* opengl texture */

struct zms_opengl_texture;

struct zms_opengl_texture* zms_opengl_texture_create_by_fd(
    struct zms_backend* backend, int fd, struct zms_screen_size size);

void zms_opengl_texture_destroy(struct zms_opengl_texture* texture);

/* opengl component */

struct zms_opengl_component_private;

struct zms_opengl_component {
  struct zms_opengl_component_private* priv;
};

struct zms_opengl_component* zms_opengl_component_create(
    struct zms_virtual_object* virtual_object);

void zms_opengl_component_destroy(struct zms_opengl_component* component);

void zms_opengl_component_attach_vertex_buffer(
    struct zms_opengl_component* component,
    struct zms_opengl_vertex_buffer* vertex_buffer);

void zms_opengl_component_attach_shader_program(
    struct zms_opengl_component* component,
    struct zms_opengl_shader_program* shader);

void zms_opengl_component_attach_texture(
    struct zms_opengl_component* component, struct zms_opengl_texture* texture);

void zms_opengl_component_texture_updated(
    struct zms_opengl_component* component);

void zms_opengl_component_set_min(
    struct zms_opengl_component* component, uint32_t min);

void zms_opengl_component_set_count(
    struct zms_opengl_component* component, uint32_t count);

void zms_opengl_component_set_topology(
    struct zms_opengl_component* component, uint32_t topology);

void zms_opengl_component_add_vertex_attribute(
    struct zms_opengl_component* component, uint32_t index, uint32_t size,
    uint32_t type, uint32_t normalized, uint32_t stride, uint32_t pointer);

/* frame callback */

struct zms_frame_callback_private;

struct zms_frame_callback {
  struct zms_frame_callback_private* priv;
  struct wl_list link;  // removed from the list when destroyed
};

typedef void (*zms_frame_callback_func_t)(void* data, uint32_t time);

struct zms_frame_callback* zms_frame_callback_create(
    struct zms_virtual_object* virtual_object, void* data,
    zms_frame_callback_func_t callback_func);

void zms_frame_callback_destroy(struct zms_frame_callback* frame_callback);

#endif  //  ZMONITORS_BACKEND_H
