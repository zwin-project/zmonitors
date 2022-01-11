#ifndef ZMONITORS_SERVER_POINTER_H
#define ZMONITORS_SERVER_POINTER_H

#include <wayland-server.h>

struct zms_pointer {
  struct wl_list resource_list;
};

struct zms_pointer* zms_pointer_create();

void zms_pointer_destroy(struct zms_pointer* pointer);

struct wl_resource* zms_pointer_resource_create(
    struct zms_pointer* pointer, struct wl_client* client, uint32_t id);

struct wl_resource* zms_pointer_inert_resource_create(
    struct wl_client* client, uint32_t id);

#endif  //  ZMONITORS_SERVER_POINTER_H
