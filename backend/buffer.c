#include "buffer.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include <zmonitors-backend.h>

static void
buffer_release(void *data, struct wl_buffer *wl_buffer)
{
  Z_UNUSED(wl_buffer);
  struct zms_buffer *buffer = data;
  buffer->writable = true;
}

static const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release,
};

static struct zms_buffer *
zms_buffer_create_by_opened_fd(struct zms_backend *backend, int fd,
    int32_t width, int32_t height, int32_t stride)
{
  struct zms_buffer *buffer;
  struct wl_buffer *proxy;
  struct wl_shm_pool *pool;
  size_t size = stride * height;

  buffer = zalloc(sizeof *buffer);
  if (buffer == NULL) goto err;

  pool = wl_shm_create_pool(backend->shm, fd, size);
  if (pool == NULL) goto err_pool;

  proxy = wl_shm_pool_create_buffer(
      pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
  if (proxy == NULL) goto err_proxy;
  wl_buffer_add_listener(proxy, &buffer_listener, buffer);

  buffer->proxy = proxy;
  buffer->pool = pool;
  buffer->size = size;
  buffer->fd = fd;
  buffer->writable = true;

  return buffer;

err_proxy:
  wl_shm_pool_destroy(pool);

err_pool:
  free(buffer);

err:
  close(fd);
  return NULL;
}

ZMS_EXPORT struct zms_buffer *
zms_buffer_create(struct zms_backend *backend, size_t size)
{
  int fd;
  fd = zms_util_create_shared_fd(size, "zmonitors-buffer");
  if (fd < 0) return NULL;

  return zms_buffer_create_by_opened_fd(backend, fd, size, 1, size);
}

ZMS_EXPORT struct zms_buffer *
zms_buffer_create_for_texture_by_fd(
    struct zms_backend *backend, int fd, int32_t width, int32_t height)
{
  int dup_fd;
  int32_t stride;
  dup_fd = fcntl(fd, F_DUPFD_CLOEXEC, 0);
  if (dup_fd < 0) return NULL;

  stride = width * sizeof(struct zms_bgra);

  return zms_buffer_create_by_opened_fd(backend, dup_fd, width, height, stride);
}

ZMS_EXPORT void
zms_buffer_destroy(struct zms_buffer *buffer)
{
  wl_buffer_destroy(buffer->proxy);
  wl_shm_pool_destroy(buffer->pool);
  close(buffer->fd);
  free(buffer);
}
