#include "frame-callback.h"

#include <zmonitors-server.h>

#include "surface.h"

static void zms_server_frame_callback_destroy(
    struct zms_server_frame_callback* frame_callback);

static void
zms_server_frame_callback_handle_destroy(struct wl_resource* resource)
{
  struct zms_server_frame_callback* frame_callback;

  frame_callback = wl_resource_get_user_data(resource);

  zms_server_frame_callback_destroy(frame_callback);
}

ZMS_EXPORT struct zms_server_frame_callback*
zms_server_frame_callback_create(struct wl_client* client, uint32_t id)
{
  struct zms_server_frame_callback* frame_callback;
  struct wl_resource* resource;

  frame_callback = zalloc(sizeof *frame_callback);
  if (frame_callback == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &wl_callback_interface, 1, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  frame_callback->resource = resource;
  wl_list_init(&frame_callback->link);

  wl_resource_set_implementation(
      resource, NULL, frame_callback, zms_server_frame_callback_handle_destroy);

  return frame_callback;

err_resource:
  free(frame_callback);

err:
  return NULL;
}

static void
zms_server_frame_callback_destroy(
    struct zms_server_frame_callback* frame_callback)
{
  wl_list_remove(&frame_callback->link);
  free(frame_callback);
}
