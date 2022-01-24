#ifndef ZMONITORS_BACKEND_DATA_OFFER_H
#define ZMONITORS_BACKEND_DATA_OFFER_H

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

struct zms_backend_data_offer {
  struct zgn_data_offer *proxy;
  void *user_data;
  const struct zms_backend_data_offer_interface *interface;
};

struct zms_backend_data_offer *zms_backend_data_offer_create(
    struct zgn_data_offer *proxy);

#endif  //  ZMONITORS_BACKEND_DATA_OFFER_H
