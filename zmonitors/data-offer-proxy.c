#include "data-offer-proxy.h"

#include <string.h>

struct zms_app_data_offer_proxy {
  struct zms_data_offer_proxy base;
  struct zms_backend_data_offer* backend_data_offer;
};

static void zms_app_data_offer_proxy_destroy(
    struct zms_app_data_offer_proxy* proxy);

static void
zms_app_data_offer_proxy_event_offer(void* user_data, const char* mime_type)
{
  struct zms_data_offer_proxy* proxy_base = user_data;
  char** type;
  type = wl_array_add(&proxy_base->mime_types, sizeof(*type));
  *type = strdup(mime_type);
}

static void
zms_app_data_offer_proxy_event_source_actions(
    void* user_data, uint32_t source_actions)
{
  struct zms_data_offer_proxy* proxy_base = user_data;
  proxy_base->source_actions = source_actions;
  proxy_base->set_source_action = true;
  zms_signal_emit(&proxy_base->source_actions_change_signal, NULL);
}

static void
zms_app_data_offer_proxy_event_action(void* user_data, uint32_t dnd_action)
{
  struct zms_data_offer_proxy* proxy_base = user_data;
  proxy_base->action = dnd_action;
  zms_signal_emit(&proxy_base->action_change_signal, NULL);
}

static const struct zms_backend_data_offer_interface data_offer_interface = {
    .offer = zms_app_data_offer_proxy_event_offer,
    .source_actions = zms_app_data_offer_proxy_event_source_actions,
    .action = zms_app_data_offer_proxy_event_action,
};

static void
zms_app_data_offer_proxy_request_destroy(
    struct zms_data_offer_proxy* proxy_base)
{
  struct zms_app_data_offer_proxy* proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_app_data_offer_proxy_destroy(proxy);
}

static void
zms_app_data_offer_proxy_request_accept(struct zms_data_offer_proxy* proxy_base,
    uint32_t serial, const char* mime_type)
{
  struct zms_app_data_offer_proxy* proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_backend_data_offer_accept(proxy->backend_data_offer, serial, mime_type);
}

static void
zms_app_data_offer_proxy_request_receive(
    struct zms_data_offer_proxy* proxy_base, const char* mime_type, int32_t fd)
{
  struct zms_app_data_offer_proxy* proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_backend_data_offer_receive(proxy->backend_data_offer, mime_type, fd);
}

static void
zms_app_data_offer_proxy_request_finish(struct zms_data_offer_proxy* proxy_base)
{
  struct zms_app_data_offer_proxy* proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_backend_data_offer_finish(proxy->backend_data_offer);
}

static void
zms_app_data_offer_proxy_request_set_actions(
    struct zms_data_offer_proxy* proxy_base, uint32_t dnd_actions,
    uint32_t preferred_action)
{
  struct zms_app_data_offer_proxy* proxy;

  proxy = wl_container_of(proxy_base, proxy, base);

  zms_backend_data_offer_set_actions(
      proxy->backend_data_offer, dnd_actions, preferred_action);
}

// constructor
ZMS_EXPORT struct zms_data_offer_proxy*
zms_app_data_offer_proxy_create(
    struct zms_backend_data_offer* backend_data_offer)
{
  struct zms_app_data_offer_proxy* proxy;

  proxy = zalloc(sizeof *proxy);
  if (proxy == NULL) goto err;

  zms_backend_data_offer_set_implementation(
      backend_data_offer, &proxy->base, &data_offer_interface);

  wl_array_init(&proxy->base.mime_types);
  proxy->base.action = WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;
  proxy->base.source_actions = WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;
  proxy->base.set_source_action = false;
  proxy->base.destroy = zms_app_data_offer_proxy_request_destroy;
  proxy->base.accept = zms_app_data_offer_proxy_request_accept;
  proxy->base.receive = zms_app_data_offer_proxy_request_receive;
  proxy->base.finish = zms_app_data_offer_proxy_request_finish;
  proxy->base.set_actions = zms_app_data_offer_proxy_request_set_actions;
  zms_signal_init(&proxy->base.destroy_signal);
  zms_signal_init(&proxy->base.action_change_signal);
  zms_signal_init(&proxy->base.source_actions_change_signal);
  proxy->backend_data_offer = backend_data_offer;

  return &proxy->base;

err:
  return NULL;
}

static void
zms_app_data_offer_proxy_destroy(struct zms_app_data_offer_proxy* proxy)
{
  char** type;
  zms_signal_emit(&proxy->base.destroy_signal, NULL);
  wl_array_for_each(type, &proxy->base.mime_types) free(*type);
  wl_array_release(&proxy->base.mime_types);
  zms_backend_data_offer_destroy(proxy->backend_data_offer);
  free(proxy);
}
