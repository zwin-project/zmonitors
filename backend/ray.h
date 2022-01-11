#ifndef ZMONITORS_BACKEND_RAY_H
#define ZMONITORS_BACKEND_RAY_H

#include <zigen-client-protocol.h>

struct zms_ray {
  void *user_data;
  const struct zms_ray_interface *interface;

  struct zgn_ray *proxy;
};

#endif  //  ZMONITORS_BACKEND_RAY_H
