#ifndef ZMONITORS_SERVER_H
#define ZMONITORS_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <wayland-server.h>
#include <zmonitors-util.h>

struct zms_compositor;

/* seat */

struct zms_seat_private;

struct zms_seat {
  struct zms_seat_private *priv;
};

/* output */

struct zms_output_private;

struct zms_output {
  struct zms_output_private *priv;
  struct wl_list link;  // -> zms_compositor.priv.output_list
};

struct zms_output *zms_output_create(struct zms_compositor *compositor,
    struct zms_screen_size size, vec2 physical_size, char *manufacturer,
    char *model);

void zms_output_destroy(struct zms_output *output);

/* view */

struct zms_view_private;

struct zms_view {
  struct zms_view_private *priv;
  struct wl_list link;  // -> zms_compositor.view_list
};

/* compositor */

struct zms_compositor_private;

struct zms_compositor {
  struct zms_compositor_private *priv;

  struct wl_display *display;
  struct wl_list view_list;

  struct zms_seat *seat;
};

struct zms_compositor *zms_compositor_create();

void zms_compositor_destroy(struct zms_compositor *compositor);

#ifdef __cplusplus
}
#endif

#endif  //  ZMONITORS_SERVER_H
