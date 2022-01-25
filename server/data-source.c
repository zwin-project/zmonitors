#include "data-source.h"

#include <zmonitors-util.h>

#include "compositor.h"

static void zms_data_source_destroy(struct zms_data_source *data_source);

static void
zms_data_source_handle_destroy(struct wl_resource *resource)
{
  struct zms_data_source *data_source;

  data_source = wl_resource_get_user_data(resource);

  zms_data_source_destroy(data_source);
}

static void
zms_data_source_protocol_offer(struct wl_client *client,
    struct wl_resource *resource, const char *mime_type)
{
  Z_UNUSED(client);
  struct zms_data_source *data_source;

  data_source = wl_resource_get_user_data(resource);

  data_source->proxy->offer(data_source->proxy, mime_type);
}

static void
zms_data_source_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static void
zms_data_source_protocol_set_actions(struct wl_client *client,
    struct wl_resource *resource, uint32_t dnd_actions)
{
  Z_UNUSED(client);
  struct zms_data_source *data_source;

  data_source = wl_resource_get_user_data(resource);

  data_source->proxy->set_actions(data_source->proxy, dnd_actions);
}

static const struct wl_data_source_interface data_source_interface = {
    .offer = zms_data_source_protocol_offer,
    .destroy = zms_data_source_protocol_destroy,
    .set_actions = zms_data_source_protocol_set_actions,
};

static void
proxy_event_target(void *data, const char *mime_type)
{
  struct zms_data_source *data_source = data;

  wl_data_source_send_target(data_source->resource, mime_type);
}

static void
proxy_event_send(void *data, const char *mime_type, int32_t fd)
{
  struct zms_data_source *data_source = data;

  wl_data_source_send_send(data_source->resource, mime_type, fd);
}

static void
proxy_event_cancelled(void *data)
{
  struct zms_data_source *data_source = data;

  wl_data_source_send_cancelled(data_source->resource);
}

static void
proxy_event_dnd_drop_performed(void *data)
{
  struct zms_data_source *data_source = data;

  wl_data_source_send_dnd_drop_performed(data_source->resource);
}

static void
proxy_event_dnd_finished(void *data)
{
  struct zms_data_source *data_source = data;

  wl_data_source_send_dnd_finished(data_source->resource);
}

static void
proxy_event_action(void *data, uint32_t dnd_action)
{
  struct zms_data_source *data_source = data;

  wl_data_source_send_action(data_source->resource, dnd_action);
}

static const struct zms_data_source_proxy_interface proxy_interface = {
    .target = proxy_event_target,
    .send = proxy_event_send,
    .cancelled = proxy_event_cancelled,
    .dnd_drop_performed = proxy_event_dnd_drop_performed,
    .dnd_finished = proxy_event_dnd_finished,
    .action = proxy_event_action,
};

ZMS_EXPORT struct zms_data_source *
zms_data_source_create(
    struct wl_client *client, uint32_t id, struct zms_compositor *compositor)
{
  struct zms_data_source *data_source;
  struct wl_resource *resource;
  struct zms_data_source_proxy *proxy;

  data_source = zalloc(sizeof *data_source);
  if (data_source == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  proxy = compositor->priv->interface->create_data_source_proxy(
      compositor->priv->user_data, data_source, &proxy_interface);
  if (proxy == NULL) {
    wl_client_post_no_memory(client);
    goto err_proxy;
  }

  resource = wl_resource_create(client, &wl_data_source_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(resource, &data_source_interface, data_source,
      zms_data_source_handle_destroy);

  data_source->resource = resource;
  data_source->proxy = proxy;

  return data_source;

err_resource:
  proxy->destroy(proxy);

err_proxy:
  free(data_source);

err:
  return NULL;
}

static void
zms_data_source_destroy(struct zms_data_source *data_source)
{
  data_source->proxy->destroy(data_source->proxy);
  free(data_source);
}
