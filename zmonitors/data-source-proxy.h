#ifndef ZMONITORS_DATA_SOURCE_PROXY_H
#define ZMONITORS_DATA_SOURCE_PROXY_H

#include <zmonitors-server.h>

#include "app.h"

struct zms_app_data_source_proxy {
  struct zms_data_source_proxy base;
  void *user_data;
  const struct zms_data_source_proxy_interface *interface;
  struct zms_backend_data_source *backend_data_source;
};

struct zms_data_source_proxy *zms_data_source_proxy_create(struct zms_app *app,
    void *proxy_user_data,
    const struct zms_data_source_proxy_interface *interface);

#endif  //  ZMONITORS_DATA_SOURCE_PROXY_H
