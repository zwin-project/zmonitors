#include "output.h"

#include <zmonitors-server.h>

#include "compositor.h"

static void
zms_output_protocol_release(
    struct wl_client* client, struct wl_resource* resource)
{
  Z_UNUSED(client);
  wl_resource_destroy(resource);
}

static const struct wl_output_interface output_interface = {
    .release = zms_output_protocol_release,
};

static void
zms_output_send_geometry(struct zms_output* output, struct wl_client* client)
{
  struct wl_resource* resource;
  int32_t x, y, width, height, refresh;

  // TODO: get geometry
  x = 0;
  y = 0;
  width = 1920;
  height = 1080;
  refresh = 60000;

  wl_resource_for_each(resource, &output->priv->resource_list)
  {
    if (wl_resource_get_client(resource) != client) continue;
    wl_output_send_geometry(resource, x, y, width, height,
        WL_OUTPUT_SUBPIXEL_UNKNOWN, "zmonitors", "virtual monitor",
        WL_OUTPUT_TRANSFORM_NORMAL);
    wl_output_send_scale(resource, 1);
    wl_output_send_mode(resource,
        WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height,
        refresh);
    wl_output_send_done(resource);
  }
}

static void
zms_seat_handle_destroy(struct wl_resource* resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void
zms_output_bind(
    struct wl_client* client, void* data, uint32_t version, uint32_t id)
{
  struct zms_output* output = data;
  struct wl_resource* resource;

  resource = wl_resource_create(client, &wl_output_interface, version, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    return;
  }

  wl_resource_set_implementation(
      resource, &output_interface, output, zms_seat_handle_destroy);

  wl_list_insert(&output->priv->resource_list, wl_resource_get_link(resource));

  zms_output_send_geometry(output, client);
}

ZMS_EXPORT struct zms_output*
zms_output_create(struct zms_compositor* compositor)
{
  struct zms_output* output;
  struct zms_output_private* priv;
  struct wl_global* global;

  output = zalloc(sizeof *output);
  if (output == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  priv = zalloc(sizeof *priv);
  if (priv == NULL) {
    zms_log("failed to allocate memory\n");
    goto err_priv;
  }

  global = wl_global_create(
      compositor->display, &wl_output_interface, 3, output, zms_output_bind);
  if (global == NULL) {
    zms_log("failed to create a output wl_global\n");
    goto err_global;
  }

  priv->global = global;
  priv->compositor = compositor;
  wl_list_init(&priv->resource_list);

  output->priv = priv;
  wl_list_insert(&compositor->priv->output_list, &output->link);

  return output;

err_global:
  free(priv);

err_priv:
  free(output);

err:
  return NULL;
}

ZMS_EXPORT void
zms_output_destroy(struct zms_output* output)
{
  struct wl_resource *resource, *tmp;

  wl_resource_for_each_safe(resource, tmp, &output->priv->resource_list)
  {
    wl_resource_set_destructor(resource, NULL);
    wl_resource_set_user_data(resource, NULL);
    wl_list_remove(wl_resource_get_link(resource));
  }

  wl_list_remove(&output->link);
  wl_global_destroy(output->priv->global);
  free(output->priv);
  free(output);
}
