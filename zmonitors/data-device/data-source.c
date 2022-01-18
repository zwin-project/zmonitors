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
  // TODO:
  zms_log("request not implemented yet: wl_data_source.offer\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(mime_type);
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

ZMS_EXPORT struct zms_data_source *
zms_data_source_create(
    struct wl_client *client, uint32_t id, struct zms_backend *backend)
{
  Z_UNUSED(backend);
  struct zms_data_source *data_source;
  struct wl_resource *resource;

  data_source = zalloc(sizeof *data_source);
  if (data_source == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &wl_data_source_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(resource, &data_source_interface, data_source,
      zms_data_source_handle_destroy);

  data_source->resource = resource;

  return data_source;

err_resource:
  free(data_source);

err:
  return NULL;
}

static void
zms_data_source_destroy(struct zms_data_source *data_source)
{
  free(data_source);
}
