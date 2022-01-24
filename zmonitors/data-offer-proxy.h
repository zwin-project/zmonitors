#ifndef ZMONITORS_DATA_OFFER_PROXY_H
#define ZMONITORS_DATA_OFFER_PROXY_H

#include <zmonitors-backend.h>
#include <zmonitors-server.h>

struct zms_data_offer_proxy* zms_app_data_offer_proxy_create(
    struct zms_backend_data_offer* backend_data_offer);

#endif  //  ZMONITORS_DATA_OFFER_PROXY_H
