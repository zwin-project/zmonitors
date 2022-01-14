#include "ray.h"

#include <zmonitors-util.h>

#include "backend.h"
#include "virtual-object.h"

static void
zms_ray_focus_virtual_object_destroy_handler(
    struct zms_listener* listener, void* data)
{
  Z_UNUSED(data);
  struct zms_ray* ray;

  ray = wl_container_of(listener, ray, focus_virtual_object_destroy_listener);

  ray->focus_virtual_object = NULL;
  wl_list_remove(&ray->focus_virtual_object_destroy_listener.link);
}

static void
zms_ray_protocol_enter(void* data, struct zgn_ray* zgn_ray, uint32_t serial,
    struct zgn_virtual_object* zgn_virtual_object, struct wl_array* origin,
    struct wl_array* direction)
{
  Z_UNUSED(zgn_ray);
  struct zms_ray* ray = data;
  struct zms_virtual_object* virtual_object;
  vec3 origin_vec, direction_vec;

  glm_vec3_from_wl_array(origin_vec, origin);
  glm_vec3_from_wl_array(direction_vec, direction);
  virtual_object = wl_proxy_get_user_data((struct wl_proxy*)zgn_virtual_object);

  if (ray->focus_virtual_object)
    wl_list_remove(&ray->focus_virtual_object_destroy_listener.link);

  ray->focus_virtual_object = virtual_object;
  zms_signal_add(&virtual_object->destroy_signal,
      &ray->focus_virtual_object_destroy_listener);

  virtual_object->interface->ray_enter(
      virtual_object->user_data, serial, origin_vec, direction_vec);
}

static void
zms_ray_protocol_leave(void* data, struct zgn_ray* zgn_ray, uint32_t serial,
    struct zgn_virtual_object* zgn_virtual_object)
{
  Z_UNUSED(zgn_ray);
  struct zms_ray* ray = data;
  struct zms_virtual_object* virtual_object;

  virtual_object = wl_proxy_get_user_data((struct wl_proxy*)zgn_virtual_object);

  if (ray->focus_virtual_object) {
    wl_list_remove(&ray->focus_virtual_object_destroy_listener.link);
    ray->focus_virtual_object = NULL;
  }

  virtual_object->interface->ray_leave(virtual_object->user_data, serial);
}

static void
zms_ray_protocol_motion(void* data, struct zgn_ray* zgn_ray, uint32_t time,
    struct wl_array* origin, struct wl_array* direction)
{
  Z_UNUSED(zgn_ray);
  struct zms_ray* ray = data;
  vec3 origin_vec, direction_vec;

  if (ray->focus_virtual_object == NULL) return;

  glm_vec3_from_wl_array(origin_vec, origin);
  glm_vec3_from_wl_array(direction_vec, direction);

  ray->focus_virtual_object->interface->ray_motion(
      ray->focus_virtual_object->user_data, time, origin_vec, direction_vec);
}

static void
zms_ray_protocol_button(void* data, struct zgn_ray* zgn_ray, uint32_t serial,
    uint32_t time, uint32_t button, uint32_t state)
{
  Z_UNUSED(zgn_ray);
  struct zms_ray* ray = data;

  if (ray->focus_virtual_object == NULL) return;

  ray->focus_virtual_object->interface->ray_button(
      ray->focus_virtual_object->user_data, serial, time, button, state);
}

static const struct zgn_ray_listener ray_listener = {
    .enter = zms_ray_protocol_enter,
    .leave = zms_ray_protocol_leave,
    .motion = zms_ray_protocol_motion,
    .button = zms_ray_protocol_button,
};

ZMS_EXPORT struct zms_ray*
zms_ray_create(struct zms_backend* backend)
{
  struct zms_ray* ray;
  struct zgn_ray* proxy;

  ray = zalloc(sizeof *ray);
  if (ray == NULL) goto err;

  proxy = zgn_seat_get_ray(backend->seat);
  if (proxy == NULL) goto err_proxy;

  zgn_ray_add_listener(proxy, &ray_listener, ray);

  ray->proxy = proxy;
  ray->focus_virtual_object_destroy_listener.notify =
      zms_ray_focus_virtual_object_destroy_handler;

  return ray;

err_proxy:
  free(ray);

err:
  return NULL;
}

ZMS_EXPORT void
zms_ray_destroy(struct zms_ray* ray)
{
  if (ray->focus_virtual_object)
    wl_list_remove(&ray->focus_virtual_object_destroy_listener.link);
  zgn_ray_release(ray->proxy);
  free(ray);
}
