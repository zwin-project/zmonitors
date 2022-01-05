#include "output.h"

#include <sys/mman.h>
#include <unistd.h>
#include <zmonitors-server.h>

#include "compositor.h"
#include "string.h"
#include "view.h"

static void draw(struct zms_output* output);

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
  int32_t x, y, refresh;

  // TODO: get geometry
  x = 0;
  y = 0;
  refresh = 60000;

  wl_resource_for_each(resource, &output->priv->resource_list)
  {
    if (wl_resource_get_client(resource) != client) continue;
    int physical_width_millimeter = output->priv->physical_size[0] * 1000;
    int physical_height_millimeter = output->priv->physical_size[1] * 1000;

    wl_output_send_geometry(resource, x, y, physical_width_millimeter,
        physical_height_millimeter, WL_OUTPUT_SUBPIXEL_UNKNOWN,
        output->priv->manufacturer, output->priv->model,
        WL_OUTPUT_TRANSFORM_NORMAL);
    wl_output_send_scale(resource, 1);
    wl_output_send_mode(resource,
        WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED,
        output->priv->size.width, output->priv->size.height, refresh);
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
zms_output_create(struct zms_compositor* compositor,
    struct zms_screen_size size, vec2 physical_size, char* manufacturer,
    char* model)
{
  struct zms_output* output;
  struct zms_output_private* priv;
  struct wl_global* global;
  int fd;
  size_t fd_size;

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

  fd_size = size.width * size.height * sizeof(struct zms_bgra);
  fd = zms_util_create_shared_fd(fd_size, "zmonitors-output");
  if (fd < 0) goto err_fd;

  global = wl_global_create(
      compositor->display, &wl_output_interface, 3, output, zms_output_bind);
  if (global == NULL) {
    zms_log("failed to create a output wl_global\n");
    goto err_global;
  }

  priv->user_data = NULL;
  priv->interface = NULL;
  priv->global = global;
  priv->compositor = compositor;
  priv->size = size;
  glm_vec3_copy(physical_size, priv->physical_size);
  priv->manufacturer = strdup(manufacturer);
  priv->model = strdup(model);
  priv->fd = fd;
  wl_list_init(&priv->resource_list);
  wl_list_init(&priv->view_list);

  output->priv = priv;
  wl_list_insert(&compositor->priv->output_list, &output->link);

  draw(output);

  return output;

err_global:
  close(fd);

err_fd:
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
  close(output->priv->fd);
  free(output->priv->model);
  free(output->priv->manufacturer);
  free(output->priv);
  free(output);
}

ZMS_EXPORT void
zms_output_set_implementation(struct zms_output* output, void* user_data,
    const struct zms_output_interface* interface)
{
  output->priv->user_data = user_data;
  output->priv->interface = interface;
}

ZMS_EXPORT void
zms_output_frame(struct zms_output* output, uint32_t time)
{
  // FIXME: send frame
  Z_UNUSED(time);
  Z_UNUSED(output);
}

ZMS_EXPORT int
zms_output_get_fd(struct zms_output* output)
{
  return output->priv->fd;
}

ZMS_EXPORT void
zms_output_map_view(struct zms_output* output, struct zms_view* view)
{
  if (view->priv->output) zms_output_unmap_view(view->priv->output, view);
  view->priv->output = output;
  wl_list_insert(output->priv->view_list.prev, &view->priv->link);
  view->priv->origin[0] = (output->priv->size.width - view->priv->width) / 2;
  view->priv->origin[1] = (output->priv->size.height - view->priv->height) / 2;

  draw(output);

  if (output->priv->interface)
    output->priv->interface->schedule_repaint(output->priv->user_data, output);
}

ZMS_EXPORT void
zms_output_unmap_view(struct zms_output* output, struct zms_view* view)
{
  assert(output == view->priv->output);

  view->priv->output = NULL;
  wl_list_remove(&view->priv->link);
  wl_list_init(&view->priv->link);

  draw(output);

  if (output->priv->interface)
    output->priv->interface->schedule_repaint(output->priv->user_data, output);
}

static void
draw(struct zms_output* output)
{
  static float rands[100];
  static int rands_initialzed = 0;
  if (!rands_initialzed) {
    for (int j = 0; j < 100; j++)
      rands[j] = (float)rand() * UINT8_MAX / RAND_MAX;
    rands_initialzed = 1;
  }

  size_t i = 0;
  struct zms_view_private* view_priv;
  struct zms_view* view;
  struct zms_screen_size size = output->priv->size;
  size_t fd_size = size.width * size.height * sizeof(struct zms_bgra);
  struct zms_bgra* data =
      mmap(NULL, fd_size, PROT_WRITE, MAP_SHARED, output->priv->fd, 0);

  for (int y = 0; y < size.height; y++) {
    for (int x = 0; x < size.width; x++) {
      int dx = (x % 100) - 50;
      int dy = (y % 100) - 50;
      int index = x / 100 + (y / 100) * 17;
      int distance = dx * dx + dy * dy;
      data[i].a = UINT8_MAX;
      if (1600 < distance && distance < 2500) {
        data[i].r = rands[(index) % 100];
        data[i].g = rands[(index + 1) % 100];
        data[i].b = rands[(index + 2) % 100];
      } else if ((1500 < distance && distance < 1600) ||
                 (2500 < distance && distance < 2600)) {
        data[i].r = (rands[(index) % 100] + UINT8_MAX) / 2;
        data[i].g = (rands[(index + 1) % 100] + UINT8_MAX) / 2;
        data[i].b = (rands[(index + 2) % 100] + UINT8_MAX) / 2;
      } else {
        data[i].r = UINT8_MAX;
        data[i].g = UINT8_MAX;
        data[i].b = UINT8_MAX;
      }
      i++;
    }
  }

  wl_list_for_each(view_priv, &output->priv->view_list, link)
  {
    view = view_priv->pub;
    int32_t origin_x = view->priv->origin[0];
    int32_t origin_y = view->priv->origin[1];
    for (int32_t y = origin_y;
         y < MIN(size.height, origin_y + view->priv->height); y++) {
      memcpy(data + origin_x + y * size.width,
          view->priv->buffer + (y - origin_y) * view->priv->width,
          MIN(view->priv->stride,
              (int32_t)sizeof(struct zms_bgra) * (size.width - origin_x)));
    }
  }

  munmap(data, fd_size);
}
