#ifndef ZMONITORS_SERVER_SURFACE_H
#define ZMONITORS_SERVER_SURFACE_H

#include <stdbool.h>
#include <wayland-server.h>
#include <zmonitors-server.h>

#include "buffer.h"
#include "compositor.h"
#include "view.h"

enum zms_surface_role {
  SURFACE_ROLE_NONE = 0,
  SURFACE_ROLE_XDG_TOPLEVEL,
  SURFACE_ROLE_XDG_POPUP,
  SURFACE_ROLE_CURSOR,
};

struct zms_surface {
  struct wl_resource *resource;

  struct zms_compositor *compositor;
  struct zms_view *view; /* nonnull */

  struct {
    bool newly_attached;
    struct zms_buffer *buffer; /* nullable */

    struct wl_list frame_callback_list;  // <- zms_frame_callback
  } pending;

  struct wl_list frame_callback_list;  // <- zms_frame_callback

  /* nullable
   * for example when a role object was once created and then destroyed */
  void *role_object;
  enum zms_surface_role role;

  // signals
  struct zms_signal commit_signal;
  struct zms_signal destroy_signal;

  // listeners
  struct wl_listener pending_buffer_destroy_listener;
};

struct zms_surface *zms_surface_create(
    struct wl_client *client, uint32_t id, struct zms_compositor *compositor);

int zms_surface_set_role(struct zms_surface *surface,
    enum zms_surface_role role, struct wl_resource *error_resource,
    uint32_t error_code);

void zms_surface_send_frame_done(struct zms_surface *surface, uint32_t time);

void zms_surface_clear_pending_buffer(struct zms_surface *surface);

#endif  //  ZMONITORS_SERVER_SURFACE_H
