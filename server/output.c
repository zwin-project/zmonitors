#include "output.h"

#include <sys/mman.h>
#include <unistd.h>
#include <zmonitors-server.h>

#include "compositor.h"
#include "math.h"
#include "pixman-helper.h"
#include "string.h"
#include "surface.h"
#include "view.h"

static void render(struct zms_output* output, pixman_region32_t* damage);

static void
draw_background(struct zms_bgra* bg, struct zms_screen_size size)
{
  struct zms_bgra edo_murasaki = {0x99, 0x53, 0x74, 0xff};
  struct zms_bgra ukon = {0x14, 0xbf, 0xfa, 0xff};
  struct zms_bgra ukon_dark = {0x0a, 0x5f, 0x7d, 0xff};

  int i = 0;
  for (int y = 0; y < size.height; y++) {
    for (int x = 0; x < size.width; x++) {
      if (x < 2 || size.width - 2 <= x || y < 2 || size.height - 2 <= y) {
        bg[i] = edo_murasaki;
      }
      i++;
    }
  }

  for (float t = 0; t < 2; t += 0.00001) {
    int x = (cos(M_PI * t * 7) * size.width + size.width) / 2;
    int y = (sin(M_PI * t * 11) * size.height + size.height) / 2;
    bg[size.width * y + x] = ukon;
    if (x - 1 >= 0) {
      bg[size.width * y + (x - 1)] = ukon_dark;
      if (y - 1 >= 0) bg[size.width * (y - 1) + (x - 1)] = ukon_dark;
      if (y + 1 < size.height) bg[size.width * (y + 1) + (x - 1)] = ukon_dark;
    }
    if (x + 1 < size.width) {
      bg[size.width * y + (x + 1)] = ukon_dark;
      if (y - 1 >= 0) bg[size.width * (y - 1) + (x + 1)] = ukon_dark;
      if (y + 1 < size.height) bg[size.width * (y + 1) + (x + 1)] = ukon_dark;
    }
  }
}

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
  size_t buffer_size;
  struct zms_bgra *buffer, *bg_buffer;
  pixman_image_t *image, *bg_image;

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

  buffer_size = size.width * size.height * sizeof(struct zms_bgra);
  fd = zms_util_create_shared_fd(buffer_size, "zmonitors-output");
  if (fd < 0) {
    zms_log("failed to create shared fd\n");
    goto err_fd;
  }

  buffer = mmap(NULL, buffer_size, PROT_WRITE, MAP_SHARED, fd, 0);
  if (buffer == MAP_FAILED) {
    zms_log("failed to mmap\n");
    goto err_mmap;
  }

  image = pixman_image_create_bits(PIXMAN_a8r8g8b8, size.width, size.height,
      (uint32_t*)buffer, size.width * sizeof(struct zms_bgra));
  if (image == NULL) {
    zms_log("failed to create pixman image\n");
    goto err_image;
  }

  bg_buffer = malloc(buffer_size);
  if (bg_buffer == NULL) {
    zms_log("failed to allocate memory\n");
    goto err_bg;
  }
  draw_background(bg_buffer, size);

  bg_image = pixman_image_create_bits(PIXMAN_a8r8g8b8, size.width, size.height,
      (uint32_t*)bg_buffer, size.width * sizeof(struct zms_bgra));

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
  priv->buffer = buffer;
  priv->image = image;
  pixman_region32_init_rect(&priv->region, 0, 0, size.width, size.height);
  wl_list_init(&priv->resource_list);
  wl_list_init(&priv->view_list);
  priv->bg_buffer = bg_buffer;
  priv->bg_image = bg_image;

  output->priv = priv;
  wl_list_insert(&compositor->priv->output_list, &output->link);

  render(output, &output->priv->region);

  return output;

err_global:
  free(bg_buffer);

err_bg:
  pixman_image_unref(image);

err_image:
  munmap(buffer, buffer_size);

err_mmap:
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

  size_t buf_size = output->priv->size.width * output->priv->size.height *
                    sizeof(struct zms_bgra);
  munmap(output->priv->buffer, buf_size);

  wl_list_remove(&output->link);
  wl_global_destroy(output->priv->global);
  pixman_image_unref(output->priv->image);
  close(output->priv->fd);
  free(output->priv->bg_buffer);
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
  struct zms_view_private* view_priv;
  wl_list_for_each(view_priv, &output->priv->view_list, link)
      zms_surface_send_frame_done(view_priv->surface, time);
}

ZMS_EXPORT int
zms_output_get_fd(struct zms_output* output)
{
  return output->priv->fd;
}

ZMS_EXPORT void
zms_output_map_view(struct zms_output* output, struct zms_view* view)
{
  pixman_region32_t damage;

  if (view->priv->output) zms_output_unmap_view(view->priv->output, view);

  view->priv->output = output;
  wl_list_insert(output->priv->view_list.prev, &view->priv->link);

  zms_view_set_origin(view,
      (output->priv->size.width - zms_view_get_width(view)) / 2,
      (output->priv->size.height - zms_view_get_height(view)) / 2);

  pixman_region32_init_view_global(&damage, view);

  render(output, &damage);

  pixman_region32_fini(&damage);

  if (output->priv->interface)
    output->priv->interface->schedule_repaint(output->priv->user_data, output);
}

ZMS_EXPORT void
zms_output_unmap_view(struct zms_output* output, struct zms_view* view)
{
  assert(output == view->priv->output);
  pixman_region32_t damage;

  if (zms_view_is_mapped(view) == false) return;

  view->priv->output = NULL;
  wl_list_remove(&view->priv->link);
  wl_list_init(&view->priv->link);

  pixman_region32_init_view_global(&damage, view);

  render(output, &damage);

  pixman_region32_fini(&damage);

  if (output->priv->interface)
    output->priv->interface->schedule_repaint(output->priv->user_data, output);
}

ZMS_EXPORT void
zms_output_update_view(struct zms_output* output, struct zms_view* view)
{
  assert(output == view->priv->output);
  pixman_region32_t damage;

  pixman_region32_init_view_global(&damage, view);

  render(output, &damage);

  pixman_region32_fini(&damage);

  if (output->priv->interface)
    output->priv->interface->schedule_repaint(output->priv->user_data, output);
}

static void
render(struct zms_output* output, pixman_region32_t* damage)
{
  struct zms_view_private* view_priv;
  struct zms_view* view;
  struct zms_screen_size size = output->priv->size;

  pixman_image_set_clip_region32(output->priv->image, damage);

  pixman_image_composite32(PIXMAN_OP_SRC, output->priv->bg_image, NULL,
      output->priv->image, 0, 0, 0, 0, 0, 0, size.width, size.height);

  pixman_image_set_clip_region32(output->priv->image, NULL);

  wl_list_for_each(view_priv, &output->priv->view_list, link)
  {
    pixman_region32_t view_region, repaint_region;
    pixman_transform_t transform;
    view = view_priv->pub;

    pixman_region32_init_view_global(&view_region, view);
    pixman_region32_init(&repaint_region);
    pixman_region32_intersect(&repaint_region, damage, &view_region);

    pixman_transform_init_view_global(&transform, view);

    pixman_image_set_clip_region32(output->priv->image, &repaint_region);

    pixman_image_set_transform(view->priv->image, &transform);

    wl_shm_buffer_begin_access(
        wl_shm_buffer_get(view->priv->buffer_ref.buffer->resource));

    pixman_image_composite32(PIXMAN_OP_OVER, view->priv->image, NULL,
        output->priv->image, 0, 0, 0, 0, 0, 0, size.width, size.height);

    wl_shm_buffer_end_access(
        wl_shm_buffer_get(view->priv->buffer_ref.buffer->resource));

    pixman_image_set_clip_region32(output->priv->image, NULL);

    pixman_region32_fini(&view_region);
    pixman_region32_fini(&repaint_region);
  }
}

ZMS_EXPORT struct zms_view*
zms_output_pick_view(
    struct zms_output* output, float x, float y, float* vx, float* vy)
{
  struct zms_view_private* view_priv;
  struct zms_view* view;
  wl_list_for_each_reverse(view_priv, &output->priv->view_list, link)
  {
    view = view_priv->pub;
    if (zms_view_contains(view, x, y, vx, vy)) return view;
  }

  return NULL;
}
