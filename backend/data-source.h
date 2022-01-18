#ifndef ZMONITORS_BACKEND_DATA_SOURCE_H
#define ZMONITORS_BACKEND_DATA_SOURCE_H

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

struct zms_backend_data_source {
  void *user_data;
  const struct zms_backend_data_source_interface *interface;
  struct zgn_data_source *proxy;
};

#endif  //  ZMONITORS_BACKEND_DATA_SOURCE_H
