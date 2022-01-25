#include "data-source-proxy.h"

#include <zmonitors-backend.h>

#include "app.h"

static void
proxy_event_target(void *data, const char *mime_type)
{
  struct zms_app_data_source_proxy *proxy = data;
  proxy->interface->target(proxy->user_data, mime_type);
}

static void
proxy_event_send(void *data, const char *mime_type, int32_t fd)
{
  struct zms_app_data_source_proxy *proxy = data;
  proxy->interface->send(proxy->user_data, mime_type, fd);
}

static void
proxy_event_cancelled(void *data)
{
  struct zms_app_data_source_proxy *proxy = data;
  proxy->interface->cancelled(proxy->user_data);
}

static void
proxy_event_dnd_drop_performed(void *data)
{
  struct zms_app_data_source_proxy *proxy = data;
  proxy->interface->dnd_drop_performed(proxy->user_data);
}

static void
proxy_event_dnd_finished(void *data)
{
  struct zms_app_data_source_proxy *proxy = data;
  proxy->interface->dnd_finished(proxy->user_data);
}

static void
proxy_event_action(void *data, uint32_t dnd_action)
{
  struct zms_app_data_source_proxy *proxy = data;
  proxy->interface->action(proxy->user_data, dnd_action);
}

static const struct zms_backend_data_source_interface
    backend_data_source_interface = {
        .target = proxy_event_target,
        .send = proxy_event_send,
        .cancelled = proxy_event_cancelled,
        .dnd_drop_performed = proxy_event_dnd_drop_performed,
        .dnd_finished = proxy_event_dnd_finished,
        .action = proxy_event_action,
};

static void
proxy_request_offer(
    struct zms_data_source_proxy *proxy_base, const char *mime_type)
{
  struct zms_app_data_source_proxy *proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_backend_data_source_offer(proxy->backend_data_source, mime_type);
}

static void
proxy_request_set_actions(
    struct zms_data_source_proxy *proxy_base, uint32_t dnd_actions)
{
  struct zms_app_data_source_proxy *proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_backend_data_source_set_actions(proxy->backend_data_source, dnd_actions);
}

// destructor
static void
proxy_request_destroy(struct zms_data_source_proxy *proxy_base)
{
  struct zms_app_data_source_proxy *proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_backend_data_source_destroy(proxy->backend_data_source);
  free(proxy);
}

// constructor
ZMS_EXPORT struct zms_data_source_proxy *
zms_data_source_proxy_create(struct zms_app *app, void *proxy_user_data,
    const struct zms_data_source_proxy_interface *interface)
{
  struct zms_app_data_source_proxy *proxy;
  struct zms_backend_data_source *backend_data_source;

  proxy = zalloc(sizeof *proxy);
  if (proxy == NULL) goto err;

  backend_data_source = zms_backend_data_source_create(
      app->backend, proxy, &backend_data_source_interface);
  if (backend_data_source == NULL) goto err_backend_data_source;

  proxy->base.offer = proxy_request_offer;
  proxy->base.set_actions = proxy_request_set_actions;
  proxy->base.destroy = proxy_request_destroy;
  proxy->user_data = proxy_user_data;
  proxy->interface = interface;
  proxy->backend_data_source = backend_data_source;

  return &proxy->base;

err_backend_data_source:
  free(proxy);

err:
  return NULL;
}
