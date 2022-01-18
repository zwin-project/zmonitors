#include "data-source.h"

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

#include "backend.h"

static void
zms_backend_data_source_protocol_target(
    void* data, struct zgn_data_source* zgn_data_source, const char* mime_type)
{
  Z_UNUSED(zgn_data_source);
  struct zms_backend_data_source* data_source = data;
  data_source->interface->target(data_source->user_data, mime_type);
}

static void
zms_backend_data_source_protocol_send(void* data,
    struct zgn_data_source* zgn_data_source, const char* mime_type, int32_t fd)
{
  Z_UNUSED(zgn_data_source);
  struct zms_backend_data_source* data_source = data;
  data_source->interface->send(data_source->user_data, mime_type, fd);
}

static void
zms_backend_data_source_protocol_cancelled(
    void* data, struct zgn_data_source* zgn_data_source)
{
  Z_UNUSED(zgn_data_source);
  struct zms_backend_data_source* data_source = data;
  data_source->interface->cancelled(data_source->user_data);
}

static void
zms_backend_data_source_protocol_dnd_drop_performed(
    void* data, struct zgn_data_source* zgn_data_source)
{
  Z_UNUSED(zgn_data_source);
  struct zms_backend_data_source* data_source = data;
  data_source->interface->dnd_drop_performed(data_source->user_data);
}

static void
zms_backend_data_source_protocol_dnd_finished(
    void* data, struct zgn_data_source* zgn_data_source)
{
  Z_UNUSED(zgn_data_source);
  struct zms_backend_data_source* data_source = data;
  data_source->interface->dnd_finished(data_source->user_data);
}

static void
zms_backend_data_source_protocol_action(
    void* data, struct zgn_data_source* zgn_data_source, uint32_t dnd_action)
{
  Z_UNUSED(zgn_data_source);
  struct zms_backend_data_source* data_source = data;
  data_source->interface->action(data_source->user_data, dnd_action);
}

static const struct zgn_data_source_listener data_source_listener = {
    .target = zms_backend_data_source_protocol_target,
    .send = zms_backend_data_source_protocol_send,
    .cancelled = zms_backend_data_source_protocol_cancelled,
    .dnd_drop_performed = zms_backend_data_source_protocol_dnd_drop_performed,
    .dnd_finished = zms_backend_data_source_protocol_dnd_finished,
    .action = zms_backend_data_source_protocol_action,
};

ZMS_EXPORT struct zms_backend_data_source*
zms_backend_data_source_create(struct zms_backend* backend, void* user_data,
    const struct zms_backend_data_source_interface* interface)
{
  struct zms_backend_data_source* data_source;
  struct zgn_data_source* proxy;

  data_source = zalloc(sizeof *data_source);
  if (data_source == NULL) goto err;

  proxy =
      zgn_data_device_manager_create_data_source(backend->data_device_manager);
  if (proxy == NULL) goto err_proxy;
  zgn_data_source_add_listener(proxy, &data_source_listener, data_source);

  data_source->user_data = user_data;
  data_source->interface = interface;
  data_source->proxy = proxy;

  return data_source;

err_proxy:
  free(data_source);

err:
  return NULL;
}

ZMS_EXPORT void
zms_backend_data_source_destroy(struct zms_backend_data_source* data_source)
{
  zgn_data_source_destroy(data_source->proxy);
  free(data_source);
}

ZMS_EXPORT void
zms_backend_data_source_offer(
    struct zms_backend_data_source* data_source, const char* mime_type)
{
  zgn_data_source_offer(data_source->proxy, mime_type);
}
