#ifndef ZMONITORS_BACKEND_BACKEND_H
#define ZMONITORS_BACKEND_BACKEND_H

#include <wayland-client.h>
#include <zigen-client-protocol.h>
#include <zigen-opengl-client-protocol.h>
#include <zigen-shell-client-protocol.h>

#include "keyboard.h"
#include "ray.h"
#include "zmonitors-backend.h"

struct zms_backend {
  void* user_data;
  const struct zms_backend_interface* interface; /* nonnull */

  struct wl_display* display;

  /* globals */
  struct zgn_compositor* compositor;
  struct zgn_seat* seat;
  struct zgn_shell* shell;
  struct wl_shm* shm;
  struct zgn_opengl* opengl;

  struct zms_ray* ray;                   /* nullable */
  struct zms_backend_keyboard* keyboard; /* nullable */
};

#endif  //  ZMONITORS_BACKEND_BACKEND_H
