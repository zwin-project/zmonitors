#ifndef ZMONITORS_SERVER_SURFACE_H
#define ZMONITORS_SERVER_SURFACE_H

#include <stdbool.h>
#include <wayland-server.h>
#include <zmonitors-server.h>

#include "compositor.h"
#include "view.h"

enum zms_surface_role {
  SURFACE_ROLE_NONE = 0,
  SURFACE_ROLE_XDG_TOPLEVEL,
  SURFACE_ROLE_XDG_POPUP,
};

struct zms_surface {
  struct wl_resource *resource;

  struct zms_compositor *compositor;
  struct zms_view *view; /* nonnull */

  struct {
    bool newly_attached;
    struct wl_resource *buffer_resource; /* nullable */
  } pending;

  /* nullable
   * for example when a role object was once created and then destroyed */
  void *role_object;
  enum zms_surface_role role;

  // signals
  struct wl_signal commit_signal;
  struct wl_signal destroy_signal;

  // listeners
  struct wl_listener pending_buffer_destroy_listener;
};

struct zms_surface *zms_surface_create(
    struct wl_client *client, uint32_t id, struct zms_compositor *compositor);

int zms_surface_set_role(struct zms_surface *surface,
    enum zms_surface_role role, struct wl_resource *error_resource,
    uint32_t error_code);

#endif  //  ZMONITORS_SERVER_SURFACE_H
