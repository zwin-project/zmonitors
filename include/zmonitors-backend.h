#ifndef ZMONITORS_BACKEND_H
#define ZMONITORS_BACKEND_H

#include <cglm/cglm.h>
#include <stdbool.h>
#include <zmonitors-util.h>

/* backend */

struct zms_backend;

struct zms_backend* zms_backend_create();

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
};

struct zms_cuboid_window* zms_cuboid_window_create(
    struct zms_backend* backend, vec3 half_size, versor quaternion);

void zms_cuboid_window_destroy(struct zms_cuboid_window* cuboid_window);

/* opengl component */

struct zms_opengl_component_private;

struct zms_opengl_component {
  struct zms_opengl_component_private* priv;
};

struct zms_opengl_component* zms_opengl_component_create(
    struct zms_virtual_object* virtual_object);

void zms_opengl_component_destroy(struct zms_opengl_component* component);

#endif  //  ZMONITORS_BACKEND_H
