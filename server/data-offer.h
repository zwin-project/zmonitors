#ifndef ZMONITORS_SERVER_DATA_OFFER_H
#define ZMONITORS_SERVER_DATA_OFFER_H

#include <wayland-server.h>
#include <zmonitors-server.h>

struct zms_data_offer {
  struct zms_weak_ref data_offer_proxy_ref;
  struct zms_data_device* data_device; /* nonnull */
  struct wl_resource* resource;

  struct zms_signal destroy_signal;
};

struct zms_data_offer* zms_data_offer_create(struct wl_client* client,
    struct zms_data_offer_proxy* data_offer_proxy,
    struct zms_data_device* data_device);

void zms_data_offer_offer(struct zms_data_offer* data_offer);

#endif  //  ZMONITORS_SERVER_DATA_OFFER_H
