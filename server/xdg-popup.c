#include "xdg-popup.h"

#include <xdg-shell-server-protocol.h>
#include <zmonitors-util.h>

static void zms_xdg_popup_destroy(struct zms_xdg_popup* popup);

static void
zms_xdg_popup_handle_destroy(struct wl_resource* resource)
{
  struct zms_xdg_popup* popup;

  popup = wl_resource_get_user_data(resource);

  zms_xdg_popup_destroy(popup);
}

static void
zms_xdg_popup_protocol_destroy(
    struct wl_client* client, struct wl_resource* resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static void
zms_xdg_popup_protocol_grab(struct wl_client* client,
    struct wl_resource* resource, struct wl_resource* seat, uint32_t serial)
{
  // TODO:
  zms_log("request not implemented yet: zms_xdg_popup.grab\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(seat);
  Z_UNUSED(serial);
}

static void
zms_xdg_popup_protocol_reposition(struct wl_client* client,
    struct wl_resource* resource, struct wl_resource* positioner,
    uint32_t token)
{
  // TODO:
  zms_log("request not implemented yet: zms_xdg_popup.reposition\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(positioner);
  Z_UNUSED(token);
}

static const struct xdg_popup_interface popup_interface = {
    .destroy = zms_xdg_popup_protocol_destroy,
    .grab = zms_xdg_popup_protocol_grab,
    .reposition = zms_xdg_popup_protocol_reposition,
};

ZMS_EXPORT struct zms_xdg_popup*
zms_xdg_popup_create(struct wl_client* client, uint32_t id)
{
  struct zms_xdg_popup* popup;
  struct wl_resource* resource;

  popup = zalloc(sizeof *popup);
  if (popup == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &xdg_popup_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(
      resource, &popup_interface, popup, zms_xdg_popup_handle_destroy);

  return popup;

err_resource:
  free(popup);

err:
  return NULL;
}

static void
zms_xdg_popup_destroy(struct zms_xdg_popup* popup)
{
  free(popup);
}
