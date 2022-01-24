#include "data-offer.h"

#include <string.h>
#include <zmonitors-util.h>

static void
zms_backend_data_offer_protocol_offer(
    void* data, struct zgn_data_offer* zgn_data_offer, const char* mime_type)
{
  Z_UNUSED(zgn_data_offer);
  struct zms_backend_data_offer* offer = data;
  offer->interface->offer(offer->user_data, mime_type);
}

static void
zms_backend_data_offer_protocol_source_action(
    void* data, struct zgn_data_offer* zgn_data_offer, uint32_t source_actions)
{
  Z_UNUSED(zgn_data_offer);
  struct zms_backend_data_offer* offer = data;
  offer->interface->source_action(offer->user_data, source_actions);
}

static void
zms_backend_data_offer_protocol_action(
    void* data, struct zgn_data_offer* zgn_data_offer, uint32_t dnd_action)
{
  Z_UNUSED(zgn_data_offer);
  struct zms_backend_data_offer* offer = data;
  offer->interface->action(offer->user_data, dnd_action);
}

static const struct zgn_data_offer_listener data_offer_listener = {
    .offer = zms_backend_data_offer_protocol_offer,
    .source_actions = zms_backend_data_offer_protocol_source_action,
    .action = zms_backend_data_offer_protocol_action,
};

ZMS_EXPORT struct zms_backend_data_offer*
zms_backend_data_offer_create(struct zgn_data_offer* proxy)
{
  struct zms_backend_data_offer* offer;

  offer = zalloc(sizeof *offer);
  if (offer == NULL) goto err;

  zgn_data_offer_add_listener(proxy, &data_offer_listener, offer);

  offer->proxy = proxy;

  return offer;

err:
  return NULL;
}

ZMS_EXPORT void
zms_backend_data_offer_destroy(struct zms_backend_data_offer* offer)
{
  zgn_data_offer_destroy(offer->proxy);
  free(offer);
}

ZMS_EXPORT void
zms_backend_data_offer_set_implementation(struct zms_backend_data_offer* offer,
    void* user_data, const struct zms_backend_data_offer_interface* interface)
{
  offer->user_data = user_data;
  offer->interface = interface;
}

ZMS_EXPORT void
zms_backend_data_offer_accept(struct zms_backend_data_offer* data_offer,
    uint32_t serial, const char* mime_type)
{
  zgn_data_offer_accept(data_offer->proxy, serial, mime_type);
}

ZMS_EXPORT void
zms_backend_data_offer_receive(struct zms_backend_data_offer* data_offer,
    const char* mime_type, int32_t fd)
{
  zgn_data_offer_receive(data_offer->proxy, mime_type, fd);
}

ZMS_EXPORT void
zms_backend_data_offer_finish(struct zms_backend_data_offer* data_offer)
{
  zgn_data_offer_finish(data_offer->proxy);
}

ZMS_EXPORT void
zms_backend_data_offer_set_actions(struct zms_backend_data_offer* data_offer,
    uint32_t dnd_actions, uint32_t preffered_action)
{
  zgn_data_offer_set_actions(data_offer->proxy, dnd_actions, preffered_action);
}
