#include "xdg-positioner.h"

#include <xdg-shell-server-protocol.h>
#include <zmonitors-util.h>

static void zms_xdg_positioner_destroy(struct zms_xdg_positioner *positioner);

static void
zms_xdg_positioner_handle_destroy(struct wl_resource *resource)
{
  struct zms_xdg_positioner *positioner;

  positioner = wl_resource_get_user_data(resource);

  zms_xdg_positioner_destroy(positioner);
}

static void
zms_xdg_positioner_protocol_destroy(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static void
zms_xdg_positioner_protocol_set_size(struct wl_client *client,
    struct wl_resource *resource, int32_t width, int32_t height)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_size\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static void
zms_xdg_positioner_protocol_set_anchor_rect(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
    int32_t height)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_anchor_rect\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(x);
  Z_UNUSED(y);
  Z_UNUSED(width);
  Z_UNUSED(height);
}

static void
zms_xdg_positioner_protocol_set_anchor(
    struct wl_client *client, struct wl_resource *resource, uint32_t anchor)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_anchor\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(anchor);
}

static void
zms_xdg_positioner_protocol_set_gravity(
    struct wl_client *client, struct wl_resource *resource, uint32_t gravity)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_gravity\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(gravity);
}

static void
zms_xdg_positioner_protocol_set_constraint_adjustment(struct wl_client *client,
    struct wl_resource *resource, uint32_t constraint_adjustment)
{
  // TODO:
  zms_log(
      "request not implemented yet: "
      "xdg_positioner_set_constraint_adjustment.\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(constraint_adjustment);
}

static void
zms_xdg_positioner_protocol_set_offset(struct wl_client *client,
    struct wl_resource *resource, int32_t x, int32_t y)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_offset\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(x);
  Z_UNUSED(y);
}

static void
zms_xdg_positioner_protocol_set_reactive(
    struct wl_client *client, struct wl_resource *resource)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_reactive\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
}

static void
zms_xdg_positioner_protocol_set_parent_size(struct wl_client *client,
    struct wl_resource *resource, int32_t parent_width, int32_t parent_height)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_parent_size\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(parent_width);
  Z_UNUSED(parent_height);
}

static void
zms_xdg_positioner_protocol_set_parent_configure(
    struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
  // TODO:
  zms_log("request not implemented yet: xdg_positioner.set_parent_configure\n");
  Z_UNUSED(client);
  Z_UNUSED(resource);
  Z_UNUSED(serial);
}

static const struct xdg_positioner_interface positioner_interface = {
    .destroy = zms_xdg_positioner_protocol_destroy,
    .set_size = zms_xdg_positioner_protocol_set_size,
    .set_anchor_rect = zms_xdg_positioner_protocol_set_anchor_rect,
    .set_anchor = zms_xdg_positioner_protocol_set_anchor,
    .set_gravity = zms_xdg_positioner_protocol_set_gravity,
    .set_constraint_adjustment =
        zms_xdg_positioner_protocol_set_constraint_adjustment,
    .set_offset = zms_xdg_positioner_protocol_set_offset,
    .set_reactive = zms_xdg_positioner_protocol_set_reactive,
    .set_parent_size = zms_xdg_positioner_protocol_set_parent_size,
    .set_parent_configure = zms_xdg_positioner_protocol_set_parent_configure,
};

ZMS_EXPORT struct zms_xdg_positioner *
zms_xdg_positioner_create(struct wl_client *client, uint32_t id)
{
  struct zms_xdg_positioner *positioner;
  struct wl_resource *resource;

  positioner = zalloc(sizeof *positioner);
  if (positioner == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  resource = wl_resource_create(client, &xdg_positioner_interface, 3, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err_resource;
  }

  wl_resource_set_implementation(resource, &positioner_interface, positioner,
      zms_xdg_positioner_handle_destroy);

  positioner->resource = resource;

  return positioner;

err_resource:
  free(positioner);

err:
  return NULL;
}

static void
zms_xdg_positioner_destroy(struct zms_xdg_positioner *positioner)
{
  free(positioner);
}
