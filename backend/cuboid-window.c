#include "cuboid-window.h"

#include <zmonitors-util.h>

#include "backend.h"
#include "virtual-object.h"

static void
zms_cuboid_window_protocol_configure(void *data,
    struct zgn_cuboid_window *zgn_cuboid_window, uint32_t serial,
    struct wl_array *half_size, struct wl_array *quaternion)
{
  // TODO:
  zms_log("event not implemented yet: zgn_cuboid_window.configure\n");
  Z_UNUSED(data);
  Z_UNUSED(zgn_cuboid_window);
  Z_UNUSED(serial);
  Z_UNUSED(half_size);
  Z_UNUSED(quaternion);
  zgn_cuboid_window_ack_configure(zgn_cuboid_window, serial);
}

static void
zms_cuboid_window_protocol_moved(void *data,
    struct zgn_cuboid_window *zgn_cuboid_window,
    struct wl_array *face_direction)
{
  // TODO:
  zms_log("event not implemented yet: zgn_cuboid_window.configure\n");
  Z_UNUSED(data);
  Z_UNUSED(zgn_cuboid_window);
  Z_UNUSED(face_direction);
}

static const struct zgn_cuboid_window_listener cuboid_window_listener = {
    .configure = zms_cuboid_window_protocol_configure,
    .moved = zms_cuboid_window_protocol_moved,
};

ZMS_EXPORT struct zms_cuboid_window *
zms_cuboid_window_create(
    struct zms_backend *backend, vec3 half_size, versor quaternion)
{
  struct zms_cuboid_window *cuboid_window;
  struct zms_virtual_object *virtual_object;
  struct zgn_cuboid_window *proxy;

  cuboid_window = zalloc(sizeof *cuboid_window);
  if (cuboid_window == NULL) goto err;

  virtual_object = zms_virtual_object_create(backend);
  if (virtual_object == NULL) goto err_virtual_object;

  {
    struct wl_array half_size_array, quaternion_array;
    wl_array_init(&half_size_array);
    wl_array_init(&quaternion_array);
    glm_vec3_to_wl_array(half_size, &half_size_array);
    glm_versor_to_wl_array(quaternion, &quaternion_array);

    proxy = zgn_shell_get_cuboid_window(backend->shell, virtual_object->proxy,
        &half_size_array, &quaternion_array);

    wl_array_release(&half_size_array);
    wl_array_release(&quaternion_array);
  }

  if (proxy == NULL) goto err_proxy;

  zgn_cuboid_window_add_listener(proxy, &cuboid_window_listener, cuboid_window);

  cuboid_window->proxy = proxy;
  wl_proxy_set_user_data((struct wl_proxy *)proxy, cuboid_window);
  cuboid_window->virtual_object = virtual_object;

  return cuboid_window;

err_proxy:
  zms_virtual_object_destroy(virtual_object);

err_virtual_object:
  free(cuboid_window);

err:
  return NULL;
}

ZMS_EXPORT void
zms_cuboid_window_destroy(struct zms_cuboid_window *cuboid_window)
{
  zms_virtual_object_destroy(cuboid_window->virtual_object);
  zgn_cuboid_window_destroy(cuboid_window->proxy);
  free(cuboid_window);
}
