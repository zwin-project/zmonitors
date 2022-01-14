#ifndef ZMONITORS_BACKEND_RAY_H
#define ZMONITORS_BACKEND_RAY_H

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

struct zms_ray {
  struct zgn_ray* proxy;
  struct zms_virtual_object* focus_virtual_object;
  struct zms_listener focus_virtual_object_destroy_listener;
};

struct zms_ray* zms_ray_create(struct zms_backend* backend);

void zms_ray_destroy(struct zms_ray* ray);

#endif  //  ZMONITORS_BACKEND_RAY_H
