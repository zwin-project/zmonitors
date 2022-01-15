#include "surface.h"

#include <zmonitors-server.h>

#include "buffer.h"
#include "frame-callback.h"
#include "output.h"
#include "pixman-helper.h"
#include "view.h"

static void zms_surface_destroy(struct zms_surface *surface);

static void
zms_surface_clear_pending_buffer(struct zms_surface *surface)
{
  if (surface->pending.buffer)
    wl_list_remove(&surface->pending_buffer_destroy_listener.link);

  surface->pending.buffer = NULL;
  surface->pending.newly_attached = false;
}

static void
zms_surface_handle_destroy(struct wl_resource *resource)
{
  struct zms_surface *surface;

  surface = wl_resource_get_user_data(resource);

  zms_surface_destroy(surface);
}

static void
zms_surface_pending_buffer_destroy_handler(
    struct wl_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_surface *surface;

  surface = wl_container_of(listener, surface, pending_buffer_destroy_listener);

  surface->pending.buffer = NULL;
  surface->pending.newly_attached = false;
  wl_list_init(&surface->pending_buffer_destroy_listener.link);
}

static void
zms_surface_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static void
zms_surface_protocol_attach(struct wl_client *client,
    struct wl_resource *resource,
    struct wl_resource *buffer_resource /* nullable */, int32_t x, int32_t y)
{
  // TODO: handle x and y args;
  Z_UNUSED(x);
  Z_UNUSED(y);
  Z_UNUSED(client);
  struct zms_surface *surface = wl_resource_get_user_data(resource);

  if (surface->pending.buffer)
    wl_list_remove(&surface->pending_buffer_destroy_listener.link);

  if (buffer_resource) {
    surface->pending.buffer = zms_buffer_create_from_resource(buffer_resource);
    wl_signal_add(&surface->pending.buffer->destroy_signal,
        &surface->pending_buffer_destroy_listener);
  } else {
    surface->pending.buffer = NULL;
  }

  surface->pending.newly_attached = true;
}

static void
zms_surface_protocol_damage(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
    int32_t height)
{
  // TODO:
  zms_log("request not implemented yet: wl_surface.damage\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(x);
  Z_UNUSED(y);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static void
zms_surface_protocol_frame(struct wl_client *client,
    struct wl_resource *resource, uint32_t callback_id)
{
  struct zms_surface *surface;
  struct zms_server_frame_callback *frame_callback;

  surface = wl_resource_get_user_data(resource);

  frame_callback = zms_server_frame_callback_create(client, callback_id);
  wl_list_insert(
      surface->pending.frame_callback_list.prev, &frame_callback->link);
}

static void
zms_surface_protocol_set_opaque_region(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *region)
{
  // TODO:
  zms_log("request not implemented yet: wl_surface.set_opaque_region\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(region);
}

static void
zms_surface_protocol_set_input_region(struct wl_client *client,
    struct wl_resource *resource, struct wl_resource *region)
{
  // TODO:
  zms_log("request not implemented yet: wl_surface.set_input_region\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(region);
}

static void
zms_surface_protocol_commit(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);
  struct zms_surface *surface;

  surface = wl_resource_get_user_data(resource);

  zms_view_commit(surface->view);

  zms_surface_clear_pending_buffer(surface);

  wl_list_insert_list(
      &surface->frame_callback_list, &surface->pending.frame_callback_list);
  wl_list_init(&surface->pending.frame_callback_list);

  zms_signal_emit(&surface->commit_signal, NULL);
}

static void
zms_surface_protocol_set_buffer_transform(
    struct wl_client *client, struct wl_resource *resource, int32_t transform)
{
  // TODO:
  zms_log("request not implemented yet: wl_surface.transform\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(transform);
}

static void
zms_surface_protocol_set_buffer_scale(
    struct wl_client *client, struct wl_resource *resource, int32_t scale)
{
  // TODO:
  zms_log("request not implemented yet: wl_surface.set_buffer_scale\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(scale);
}

static void
zms_surface_protocol_damage_buffer(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
    int32_t height)
{
  // TODO:
  zms_log("request not implemented yet: wl_surface.damage_buffer\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(x);
  Z_UNUSED(y);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static const struct wl_surface_interface surface_interface = {
    .destroy = zms_surface_protocol_destroy,
    .attach = zms_surface_protocol_attach,
    .damage = zms_surface_protocol_damage,
    .frame = zms_surface_protocol_frame,
    .set_opaque_region = zms_surface_protocol_set_opaque_region,
    .set_input_region = zms_surface_protocol_set_input_region,
    .commit = zms_surface_protocol_commit,
    .set_buffer_transform = zms_surface_protocol_set_buffer_transform,
    .set_buffer_scale = zms_surface_protocol_set_buffer_scale,
    .damage_buffer = zms_surface_protocol_damage_buffer,
};

ZMS_EXPORT struct zms_surface *
zms_surface_create(
    struct wl_client *client, uint32_t id, struct zms_compositor *compositor)
{
  struct zms_surface *surface;
  struct wl_resource *resource;
  struct zms_view *view;

  surface = zalloc(sizeof *surface);
  if (surface == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  surface->compositor = compositor;
  surface->role_object = NULL;
  surface->role = SURFACE_ROLE_NONE;

  view = zms_view_create(surface);
  if (view == NULL) {
    wl_client_post_no_memory(client);
    goto err_view;
  }

  resource = wl_resource_create(client, &wl_surface_interface, 5, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(
      resource, &surface_interface, surface, zms_surface_handle_destroy);

  surface->resource = resource;
  surface->view = view;
  surface->pending.buffer = NULL;
  surface->pending.newly_attached = false;
  wl_list_init(&surface->pending.frame_callback_list);
  wl_list_init(&surface->frame_callback_list);
  zms_signal_init(&surface->commit_signal);
  zms_signal_init(&surface->destroy_signal);

  surface->pending_buffer_destroy_listener.notify =
      zms_surface_pending_buffer_destroy_handler;

  return surface;

err_resource:
  zms_view_destroy(view);

err_view:
  free(surface);

err:
  return NULL;
}

static void
zms_surface_destroy(struct zms_surface *surface)
{
  struct zms_server_frame_callback *frame_callback, *tmp;

  wl_list_for_each_safe(
      frame_callback, tmp, &surface->pending.frame_callback_list, link)
      wl_resource_destroy(frame_callback->resource);

  wl_list_for_each_safe(frame_callback, tmp, &surface->frame_callback_list,
      link) wl_resource_destroy(frame_callback->resource);

  if (surface->pending.buffer)
    wl_list_remove(&surface->pending_buffer_destroy_listener.link);
  zms_signal_emit(&surface->destroy_signal, NULL);
  zms_view_destroy(surface->view);
  free(surface);
}

static const char *
role_to_role_name(enum zms_surface_role role)
{
  switch (role) {
    case SURFACE_ROLE_NONE:
      return "none";

    case SURFACE_ROLE_XDG_POPUP:
      return "xdg_popup";

    case SURFACE_ROLE_XDG_TOPLEVEL:
      return "xdg_toplevel";

    case SURFACE_ROLE_CURSOR:
      return "cursor";
  }
  assert(false && "not reached");
}

ZMS_EXPORT int
zms_surface_set_role(struct zms_surface *surface, enum zms_surface_role role,
    struct wl_resource *error_resource, uint32_t error_code)
{
  if (surface->role == SURFACE_ROLE_NONE || surface->role == role) {
    surface->role = role;
    return 0;
  }

  wl_resource_post_error(error_resource, error_code,
      "Cannot assign role %s to wl_surface@%d, already had role %s\n",
      role_to_role_name(role), wl_resource_get_id(surface->resource),
      role_to_role_name(surface->role));

  return -1;
}

ZMS_EXPORT void
zms_surface_send_frame_done(struct zms_surface *surface, uint32_t time)
{
  struct zms_server_frame_callback *frame_callback, *tmp;

  wl_list_for_each_safe(
      frame_callback, tmp, &surface->frame_callback_list, link)
  {
    wl_callback_send_done(frame_callback->resource, time);
    wl_resource_destroy(frame_callback->resource);
  }
  wl_client_flush(wl_resource_get_client(surface->resource));
}
