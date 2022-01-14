#ifndef ZMONITORS_SERVER_FRAME_CALLBACK_H
#define ZMONITORS_SERVER_FRAME_CALLBACK_H

#include <wayland-server.h>

struct zms_server_frame_callback {
  struct wl_resource* resource;
  struct wl_list link;
};

struct zms_server_frame_callback* zms_server_frame_callback_create(
    struct wl_client* client, uint32_t id);

#endif  //  ZMONITORS_SERVER_FRAME_CALLBACK_H
