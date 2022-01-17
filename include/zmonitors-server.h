#ifndef ZMONITORS_SERVER_H
#define ZMONITORS_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <wayland-server.h>
#include <zmonitors-util.h>

struct zms_compositor;

/* pixel buffer */

struct zms_pixel_buffer_private;

struct zms_pixel_buffer {
  struct zms_pixel_buffer_private *priv;

  int fd;
  uint32_t width, height, stride;
  size_t size;
  void *user_data;
};

struct zms_pixel_buffer *zms_pixel_buffer_create(
    uint32_t width, uint32_t height, void *user_data);

/* output */

struct zms_output;
struct zms_output_private;

struct zms_output_interface {
  void (*schedule_repaint)(void *user_data, struct zms_output *output);
};

struct zms_output {
  struct zms_output_private *priv;
  struct wl_list link;  // -> zms_compositor.priv.output_list
  int pixel_buffer_count;
  struct zms_pixel_buffer **pixel_buffers;
};

struct zms_output *zms_output_create(struct zms_compositor *compositor,
    struct zms_screen_size size, vec2 physical_size, char *manufacturer,
    char *model);

void zms_output_destroy(struct zms_output *output);

void zms_output_set_implementation(struct zms_output *output, void *user_data,
    const struct zms_output_interface *interface);

/**
 * @return previously-used back buffer (next front buffer)
 */
struct zms_pixel_buffer *zms_output_buffer_ring_rotate(
    struct zms_output *output);

void zms_output_frame(struct zms_output *output, uint32_t time);

/* seat */

struct zms_seat_private;

struct zms_seat {
  struct zms_seat_private *priv;
};

void zms_seat_init_pointer(struct zms_seat *seat);

void zms_seat_release_pointer(struct zms_seat *seat);

void zms_seat_notify_pointer_motion_abs(
    struct zms_seat *seat, struct zms_output *output, vec2 pos, uint32_t time);

void zms_seat_notify_pointer_button(struct zms_seat *seat, uint32_t time,
    uint32_t button, uint32_t state, uint32_t serial);

void zms_seat_notify_pointer_leave(struct zms_seat *seat);

/* view */

struct zms_view_private;

struct zms_view {
  struct zms_view_private *priv;

  // signals
  struct zms_signal destroy_signal;
  struct zms_signal unmap_signal; /* invoked also when destroyed; */
};

/* compositor */

struct zms_compositor_private;

struct zms_compositor {
  struct zms_compositor_private *priv;

  struct wl_display *display;

  struct zms_seat *seat;
};

struct zms_compositor *zms_compositor_create();

void zms_compositor_destroy(struct zms_compositor *compositor);

#ifdef __cplusplus
}
#endif

#endif  //  ZMONITORS_SERVER_H
