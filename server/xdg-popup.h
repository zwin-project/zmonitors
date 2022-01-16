#ifndef ZMONITORS_SERVER_XDG_POPUP_H
#define ZMONITORS_SERVER_XDG_POPUP_H

#include <wayland-server.h>

struct zms_xdg_popup {
  struct wl_resource* resource;
};

struct zms_xdg_popup* zms_xdg_popup_create(
    struct wl_client* client, uint32_t id);

#endif  //  ZMONITORS_SERVER_XDG_POPUP_H
