#include "ray.h"

#include <zmonitors-util.h>

#include "backend.h"

static void
zms_ray_protocol_enter(void* data, struct zgn_ray* ray, uint32_t serial,
    struct zgn_virtual_object* virtual_object, struct wl_array* origin,
    struct wl_array* direction)
{
  Z_UNUSED(data);
  Z_UNUSED(ray);
  Z_UNUSED(serial);
  Z_UNUSED(virtual_object);
  Z_UNUSED(origin);
  Z_UNUSED(direction);
}

static void
zms_ray_protocol_leave(void* data, struct zgn_ray* ray, uint32_t serial,
    struct zgn_virtual_object* virtual_object)
{
  Z_UNUSED(data);
  Z_UNUSED(ray);
  Z_UNUSED(serial);
  Z_UNUSED(virtual_object);
}

static void
zms_ray_protocol_motion(void* data, struct zgn_ray* ray, uint32_t time,
    struct wl_array* origin, struct wl_array* direction)
{
  Z_UNUSED(data);
  Z_UNUSED(ray);
  Z_UNUSED(time);
  Z_UNUSED(origin);
  Z_UNUSED(direction);
}

static void
zms_ray_protocol_button(void* data, struct zgn_ray* ray, uint32_t serial,
    uint32_t time, uint32_t button, uint32_t state)
{
  Z_UNUSED(data);
  Z_UNUSED(ray);
  Z_UNUSED(serial);
  Z_UNUSED(time);
  Z_UNUSED(button);
  Z_UNUSED(state);
}

static const struct zgn_ray_listener ray_listener = {
    .enter = zms_ray_protocol_enter,
    .leave = zms_ray_protocol_leave,
    .motion = zms_ray_protocol_motion,
    .button = zms_ray_protocol_button,
};

ZMS_EXPORT struct zms_ray*
zms_ray_create(struct zms_backend* backend, void* user_data,
    const struct zms_ray_interface* interface)
{
  struct zms_ray* ray;
  struct zgn_ray* proxy;

  ray = zalloc(sizeof *ray);
  if (ray == NULL) goto err;

  proxy = zgn_seat_get_ray(backend->seat);
  if (proxy == NULL) goto err_proxy;

  zgn_ray_add_listener(proxy, &ray_listener, ray);

  ray->user_data = user_data;
  ray->interface = interface;
  ray->proxy = proxy;

  return ray;

err_proxy:
  free(ray);

err:
  return NULL;
}

ZMS_EXPORT void
zms_ray_destroy(struct zms_ray* ray)
{
  zgn_ray_release(ray->proxy);
  free(ray);
}
