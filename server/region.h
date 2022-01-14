#ifndef ZMONITORS_SERVER_REGION_H
#define ZMONITORS_SERVER_REGION_H

#include <wayland-server.h>

struct zms_region {
  struct wl_resource *resource;
};

struct zms_region *zms_region_create(struct wl_client *client, uint32_t id);

#endif  //  ZMONITORS_SERVER_REGION_H
