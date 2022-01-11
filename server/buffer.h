#ifndef ZMONITORS_SERVER_BUFFER_H
#define ZMONITORS_SERVER_BUFFER_H

#include <wayland-server.h>

struct zms_buffer {
  struct wl_resource *resource;
  struct wl_listener resource_destroy_listener;

  struct wl_signal destroy_signal;

  uint32_t busy_count;
};

struct zms_buffer_ref {
  struct zms_buffer *buffer;
  struct wl_listener buffer_destroy_listener;
};

struct zms_buffer *zms_buffer_create_from_resource(
    struct wl_resource *resource);

void zms_buffer_reference(
    struct zms_buffer_ref *ref, struct zms_buffer *buffer /* nullable */);

#endif  //  ZMONITORS_SERVER_BUFFER_H
