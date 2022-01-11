#ifndef ZMONITORS_SERVER_POINTER_H
#define ZMONITORS_SERVER_POINTER_H

#include <wayland-server.h>

struct zms_pointer {
  struct wl_list resource_list;
};

struct zms_pointer* zms_pointer_create();

void zms_pointer_destroy(struct zms_pointer* pointer);

#endif  //  ZMONITORS_SERVER_POINTER_H
