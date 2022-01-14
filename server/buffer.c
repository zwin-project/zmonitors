#include "buffer.h"

#include <zmonitors-server.h>

static void zms_buffer_destroy(struct zms_buffer *buffer);

static void
zms_buffer_resource_destroy_handler(struct wl_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_buffer *buffer =
      wl_container_of(listener, buffer, resource_destroy_listener);

  zms_buffer_destroy(buffer);
}

ZMS_EXPORT struct zms_buffer *
zms_buffer_create_from_resource(struct wl_resource *resource)
{
  struct zms_buffer *buffer;

  buffer = zalloc(sizeof *buffer);
  if (buffer == NULL) goto err;

  buffer->resource = resource;
  buffer->resource_destroy_listener.notify =
      zms_buffer_resource_destroy_handler;
  wl_resource_add_destroy_listener(
      resource, &buffer->resource_destroy_listener);
  wl_signal_init(&buffer->destroy_signal);

  return buffer;

err:

  return NULL;
}

static void
zms_buffer_destroy(struct zms_buffer *buffer)
{
  wl_signal_emit(&buffer->destroy_signal, NULL);
  free(buffer);
}

static void
zms_buffer_ref_buffer_destroy_handler(struct wl_listener *listener, void *data)
{
  Z_UNUSED(data);

  struct zms_buffer_ref *ref =
      wl_container_of(listener, ref, buffer_destroy_listener);

  ref->buffer = NULL;
}

ZMS_EXPORT void
zms_buffer_reference(
    struct zms_buffer_ref *ref, struct zms_buffer *buffer /* nullable */)
{
  if (ref->buffer && buffer != ref->buffer) {
    ref->buffer->busy_count--;
    if (ref->buffer->busy_count == 0) {
      wl_buffer_send_release(ref->buffer->resource);
    }
    wl_list_remove(&ref->buffer_destroy_listener.link);
  }

  if (buffer && buffer != ref->buffer) {
    buffer->busy_count++;
    wl_signal_add(&buffer->destroy_signal, &ref->buffer_destroy_listener);
  }

  ref->buffer = buffer;
  ref->buffer_destroy_listener.notify = zms_buffer_ref_buffer_destroy_handler;
}
