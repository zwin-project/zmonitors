#ifndef ZMONITORS_SERVER_PIXEL_BUFFER_H
#define ZMONITORS_SERVER_PIXEL_BUFFER_H

#include <pixman-1/pixman.h>
#include <zmonitors-server.h>

struct zms_pixel_buffer_private {
  void *buffer;
  pixman_image_t *image;
};

void zms_pixel_buffer_destroy(struct zms_pixel_buffer *pixel_buffer);

#endif  //  ZMONITORS_SERVER_PIXEL_BUFFER_H
