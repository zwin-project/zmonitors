#include "cuboid-window.h"

#include <zmonitors-util.h>

#include "backend.h"
#include "virtual-object.h"

static void
zms_cuboid_window_protocol_configure(void *data,
    struct zgn_cuboid_window *zgn_cuboid_window, uint32_t serial,
    struct wl_array *half_size_array, struct wl_array *quaternion_array)
{
  struct zms_cuboid_window *cuboid_window = data;

  glm_vec3_from_wl_array(cuboid_window->half_size, half_size_array);
  glm_versor_from_wl_array(cuboid_window->quaternion, quaternion_array);

  zgn_cuboid_window_ack_configure(zgn_cuboid_window, serial);

  if (cuboid_window->configured)
    cuboid_window->configured(cuboid_window->priv->user_data, cuboid_window);
}

static void
zms_cuboid_window_protocol_moved(void *data,
    struct zgn_cuboid_window *zgn_cuboid_window,
    struct wl_array *face_direction)
{
  Z_UNUSED(zgn_cuboid_window);
  struct zms_cuboid_window *cuboid_window = data;
  vec3 face_direction_vec;

  glm_vec3_from_wl_array(face_direction_vec, face_direction);

  cuboid_window->priv->interface->moved(
      cuboid_window->priv->user_data, face_direction_vec);
}

static const struct zgn_cuboid_window_listener cuboid_window_listener = {
    .configure = zms_cuboid_window_protocol_configure,
    .moved = zms_cuboid_window_protocol_moved,
};

ZMS_EXPORT struct zms_cuboid_window *
zms_cuboid_window_create(void *user_data,
    const struct zms_cuboid_window_interface *interface,
    struct zms_backend *backend, vec3 half_size, versor quaternion)
{
  struct zms_cuboid_window *cuboid_window;
  struct zms_cuboid_window_private *priv;
  struct zms_virtual_object *virtual_object;
  struct zgn_cuboid_window *proxy;

  cuboid_window = zalloc(sizeof *cuboid_window);
  if (cuboid_window == NULL) goto err;

  priv = zalloc(sizeof *priv);
  if (priv == NULL) goto err_priv;

  virtual_object =
      zms_virtual_object_create(user_data, &interface->base, backend);
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

  priv->proxy = proxy;
  wl_proxy_set_user_data((struct wl_proxy *)proxy, cuboid_window);
  priv->user_data = user_data;
  priv->interface = interface;
  cuboid_window->priv = priv;
  cuboid_window->backend = backend;
  cuboid_window->virtual_object = virtual_object;

  glm_vec3_zero(cuboid_window->half_size);
  glm_quat_identity(cuboid_window->quaternion);

  cuboid_window->configured = NULL;

  return cuboid_window;

err_proxy:
  zms_virtual_object_destroy(virtual_object);

err_virtual_object:
  free(priv);

err_priv:
  free(cuboid_window);

err:
  return NULL;
}

ZMS_EXPORT void
zms_cuboid_window_destroy(struct zms_cuboid_window *cuboid_window)
{
  zgn_cuboid_window_destroy(cuboid_window->priv->proxy);
  zms_virtual_object_destroy(cuboid_window->virtual_object);
  free(cuboid_window->priv);
  free(cuboid_window);
}

ZMS_EXPORT void
zms_cuboid_window_commit(struct zms_cuboid_window *cuboid_window)
{
  zgn_virtual_object_commit(cuboid_window->virtual_object->proxy);
}

ZMS_EXPORT void
zms_cuboid_window_move(struct zms_cuboid_window *cuboid_window, uint32_t serial)
{
  zgn_cuboid_window_move(
      cuboid_window->priv->proxy, cuboid_window->backend->seat, serial);
  zms_backend_flush(cuboid_window->backend);
}

ZMS_EXPORT void
zms_cuboid_window_rotate(
    struct zms_cuboid_window *cuboid_window, versor quaternion)
{
  struct wl_array array;
  wl_array_init(&array);
  glm_versor_to_wl_array(quaternion, &array);

  zgn_cuboid_window_rotate(cuboid_window->priv->proxy, &array);
  zms_backend_flush(cuboid_window->backend);

  wl_array_release(&array);
}
