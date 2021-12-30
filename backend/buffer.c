#include "buffer.h"

#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include <zmonitors-backend.h>

static int
create_shared_fd(off_t size)
{
  const char *name = "zms-buffer";
  int fd = memfd_create(name, MFD_CLOEXEC | MFD_ALLOW_SEALING);
  if (fd < 0) return fd;
  unlink(name);

  if (ftruncate(fd, size) < 0) {
    close(fd);
    return -1;
  }

  return fd;
}

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

ZMS_EXPORT struct zms_buffer *
zms_buffer_create(struct zms_backend *backend, size_t size)
{
  struct zms_buffer *buffer;
  struct wl_buffer *proxy;
  struct wl_shm_pool *pool;
  int fd;

  buffer = zalloc(sizeof *buffer);
  if (buffer == NULL) goto err;

  fd = create_shared_fd(size);
  if (fd < 0) goto err_fd;

  pool = wl_shm_create_pool(backend->shm, fd, size);
  if (pool == NULL) goto err_pool;

  proxy = wl_shm_pool_create_buffer(pool, 0, size, 1, size, 0);
  if (proxy == NULL) goto err_proxy;
  wl_buffer_add_listener(proxy, &buffer_listener, buffer);

  buffer->proxy = proxy;
  buffer->pool = pool;
  buffer->fd = fd;
  buffer->writable = true;

  return buffer;

err_proxy:
  wl_shm_pool_destroy(pool);

err_pool:
  close(fd);

err_fd:
  free(buffer);

err:
  return NULL;
}

ZMS_EXPORT void
zms_buffer_destroy(struct zms_buffer *buffer)
{
  wl_buffer_destroy(buffer->proxy);
  wl_shm_pool_destroy(buffer->pool);
  close(buffer->fd);
  free(buffer);
}
