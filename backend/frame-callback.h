#ifndef ZMONITORS_BACKEND_FRAME_CALLBACK_H
#define ZMONITORS_BACKEND_FRAME_CALLBACK_H

#include <wayland-client.h>
#include <zmonitors-backend.h>

struct zms_frame_callback_private {
  struct zms_frame_callback *pub;
  void *user_data;
  zms_frame_callback_func_t user_func;
  struct wl_callback *callback;
  struct zms_listener virtual_object_destroy_listener;
};

#endif  //  ZMONITORS_BACKEND_FRAME_CALLBACK_H
