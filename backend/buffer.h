#ifndef ZMONITORS_BACKEND_BUFFER_H
#define ZMONITORS_BACKEND_BUFFER_H

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

#include "backend.h"

struct zms_buffer {
  struct wl_buffer *proxy;
  struct wl_shm_pool *pool;
  int fd;
  bool writable;
};

struct zms_buffer *zms_buffer_create(struct zms_backend *backend, size_t size);

void zms_buffer_destroy(struct zms_buffer *buffer);

#endif  //  ZMONITORS_BACKEND_BUFFER_H
