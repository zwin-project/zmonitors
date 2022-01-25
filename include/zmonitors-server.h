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

/* data source */

struct zms_data_source_proxy_interface {
  void (*target)(void *data, const char *mime_type);
  void (*send)(void *data, const char *mime_type, int32_t fd);
  void (*cancelled)(void *data);
  void (*dnd_drop_performed)(void *data);
  void (*dnd_finished)(void *data);
  void (*action)(void *data, uint32_t dnd_action);
};

struct zms_data_source_proxy {
  void (*offer)(struct zms_data_source_proxy *proxy, const char *mime_type);
  void (*set_actions)(
      struct zms_data_source_proxy *proxy, uint32_t dnd_actions);
  void (*destroy)(struct zms_data_source_proxy *proxy);
};

/* data offer */

struct zms_data_offer_proxy {
  struct wl_array mime_types;
  uint32_t action;
  uint32_t source_actions;
  bool set_source_action;
  void (*accept)(struct zms_data_offer_proxy *offer_proxy, uint32_t serial,
      const char *mime_type);
  void (*receive)(struct zms_data_offer_proxy *offer_proxy,
      const char *mime_type, int32_t fd);
  void (*finish)(struct zms_data_offer_proxy *offer_proxy);
  void (*set_actions)(struct zms_data_offer_proxy *offer_proxy,
      uint32_t dnd_actions, uint32_t preferred_action);
  void (*destroy)(struct zms_data_offer_proxy *offer_proxy);

  // signals
  struct zms_signal destroy_signal;
  struct zms_signal action_change_signal;
  struct zms_signal source_actions_change_signal;
};

/* output */

struct zms_output;
struct zms_output_private;

struct zms_output_interface {
  void (*schedule_repaint)(void *user_data, struct zms_output *output);
  void (*start_drag)(void *user_data,
      struct zms_data_source_proxy *data_source_proxy, uint32_t serial);
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

/* data device */

struct zms_data_device;

void zms_data_device_notify_new_data_offer(struct zms_data_device *data_device,
    struct zms_data_offer_proxy *data_offer_proxy);

void zms_data_device_notify_drop(struct zms_data_device *data_device);

void zms_data_device_notify_leave(struct zms_data_device *data_device);

void zms_data_device_destroy_data_offer(struct zms_data_device *data_device);

/* seat */

struct zms_seat_private;

struct zms_seat {
  struct zms_seat_private *priv;
  struct zms_data_device *data_device; /* nonnull */
};

void zms_seat_init_pointer(struct zms_seat *seat);

void zms_seat_release_pointer(struct zms_seat *seat);

void zms_seat_notify_pointer_enter(
    struct zms_seat *seat, struct zms_output *output, vec2 pos);

void zms_seat_notify_pointer_motion_abs(
    struct zms_seat *seat, struct zms_output *output, vec2 pos, uint32_t time);

void zms_seat_notify_pointer_button(struct zms_seat *seat, uint32_t time,
    uint32_t button, uint32_t state, uint32_t serial);

void zms_seat_notify_pointer_leave(struct zms_seat *seat);

void zms_seat_notify_start_drag(struct zms_seat *seat, uint32_t enter_serial);

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

struct zms_compositor_interface {
  struct zms_data_source_proxy *(*create_data_source_proxy)(void *data,
      void *proxy_user_data,
      const struct zms_data_source_proxy_interface *interface);
};

struct zms_compositor {
  struct zms_compositor_private *priv;

  struct wl_display *display;

  struct zms_seat *seat;
};

struct zms_compositor *zms_compositor_create(
    void *user_data, const struct zms_compositor_interface *interface);

void zms_compositor_destroy(struct zms_compositor *compositor);

#ifdef __cplusplus
}
#endif

#endif  //  ZMONITORS_SERVER_H
