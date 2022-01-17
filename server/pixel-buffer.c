#include "pixel-buffer.h"

#include <sys/mman.h>
#include <unistd.h>
#include <zmonitors-server.h>

ZMS_EXPORT struct zms_pixel_buffer *
zms_pixel_buffer_create(uint32_t width, uint32_t height, void *user_data)
{
  struct zms_pixel_buffer *pixel_buffer;
  struct zms_pixel_buffer_private *priv;
  pixman_image_t *image;
  uint32_t stride = width * sizeof(struct zms_bgra);
  uint32_t size = stride * height;
  int fd;
  void *buffer;

  pixel_buffer = zalloc(sizeof *pixel_buffer);
  if (pixel_buffer == NULL) goto err;

  priv = zalloc(sizeof *priv);
  if (priv == NULL) goto err_priv;

  fd = zms_util_create_shared_fd(size, "zmonitors-pixel-buffer");
  if (fd < 0) goto err_fd;

  buffer = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
  if (buffer == MAP_FAILED) goto err_buffer;

  image =
      pixman_image_create_bits(PIXMAN_a8r8g8b8, width, height, buffer, stride);
  if (image == NULL) goto err_image;

  priv->buffer = buffer;
  priv->image = image;
  pixel_buffer->priv = priv;
  pixel_buffer->fd = fd;
  pixel_buffer->width = width;
  pixel_buffer->height = height;
  pixel_buffer->stride = stride;
  pixel_buffer->size = size;
  pixel_buffer->user_data = user_data;

  return pixel_buffer;

err_image:
  munmap(buffer, size);

err_buffer:
  close(fd);

err_fd:
  free(priv);

err_priv:
  free(pixel_buffer);

err:
  return NULL;
}

ZMS_EXPORT void
zms_pixel_buffer_destroy(struct zms_pixel_buffer *pixel_buffer)
{
  pixman_image_unref(pixel_buffer->priv->image);
  munmap(pixel_buffer->priv->buffer, pixel_buffer->size);
  close(pixel_buffer->fd);
  free(pixel_buffer->priv);
  free(pixel_buffer);
}
