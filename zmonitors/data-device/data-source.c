#include "data-source.h"

#include <zmonitors-server.h>

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
  zms_backend_data_source_offer(data_source->proxy, mime_type);
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
  // TODO:
  zms_log("request not implemented yet: wl_data_source.set_actions\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(dnd_actions);
}

static const struct wl_data_source_interface data_source_interface = {
    .offer = zms_data_source_protocol_offer,
    .destroy = zms_data_source_protocol_destroy,
    .set_actions = zms_data_source_protocol_set_actions,
};

static void
zms_data_source_proxy_target(void *data, const char *mime_type)
{
  // TODO:
  zms_log("event not implemented yet: zgn_data_source.target\n");
  struct zms_data_source *data_source = data;
  Z_UNUSED(data_source);
  Z_UNUSED(mime_type);
}

static void
zms_data_source_proxy_send(void *data, const char *mime_type, int32_t fd)
{
  // TODO:
  zms_log("event not implemented yet: zgn_data_source.send\n");
  struct zms_data_source *data_source = data;
  Z_UNUSED(data_source);
  Z_UNUSED(mime_type);
  Z_UNUSED(fd);
}

static void
zms_data_source_proxy_cancelled(void *data)
{
  // TODO:
  zms_log("event not implemented yet: zgn_data_source.cancelled\n");
  struct zms_data_source *data_source = data;
  Z_UNUSED(data_source);
}

static void
zms_data_source_proxy_dnd_drop_performed(void *data)
{
  // TODO:
  zms_log("event not implemented yet: zgn_data_source.dnd_drop_performed\n");
  struct zms_data_source *data_source = data;
  Z_UNUSED(data_source);
}

static void
zms_data_source_proxy_dnd_finished(void *data)
{
  // TODO:
  zms_log("event not implemented yet: zgn_data_source.dnd_finished\n");
  struct zms_data_source *data_source = data;
  Z_UNUSED(data_source);
}

static void
zms_data_source_proxy_action(void *data, uint32_t dnd_action)
{
  // TODO:
  zms_log("event not implemented yet: zgn_data_source.action\n");
  struct zms_data_source *data_source = data;
  Z_UNUSED(data_source);
  Z_UNUSED(dnd_action);
}

static const struct zms_backend_data_source_interface backend_interface = {
    .target = zms_data_source_proxy_target,
    .send = zms_data_source_proxy_send,
    .cancelled = zms_data_source_proxy_cancelled,
    .dnd_drop_performed = zms_data_source_proxy_dnd_drop_performed,
    .dnd_finished = zms_data_source_proxy_dnd_finished,
    .action = zms_data_source_proxy_action,
};

ZMS_EXPORT struct zms_data_source *
zms_data_source_create(
    struct wl_client *client, uint32_t id, struct zms_backend *backend)
{
  Z_UNUSED(backend);
  struct zms_data_source *data_source;
  struct wl_resource *resource;
  struct zms_backend_data_source *proxy;

  data_source = zalloc(sizeof *data_source);
  if (data_source == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  proxy =
      zms_backend_data_source_create(backend, data_source, &backend_interface);
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
  zms_backend_data_source_destroy(proxy);

err_proxy:
  free(data_source);

err:
  return NULL;
}

static void
zms_data_source_destroy(struct zms_data_source *data_source)
{
  zms_backend_data_source_destroy(data_source->proxy);
  free(data_source);
}
