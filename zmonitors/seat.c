#include "seat.h"

#include <zigen-client-protocol.h>
#include <zmonitors-util.h>

#include "app.h"

static void
ray_enter(void* data, uint32_t serial,
    struct zms_virtual_object* virtual_object, vec3 origin, vec3 direction)
{
  struct zms_app* app = data;
  Z_UNUSED(app);
  Z_UNUSED(serial);
  Z_UNUSED(virtual_object);
  Z_UNUSED(origin);
  Z_UNUSED(direction);
}

static void
ray_leave(
    void* data, uint32_t serial, struct zms_virtual_object* virtual_object)
{
  struct zms_app* app = data;
  Z_UNUSED(app);
  Z_UNUSED(serial);
  Z_UNUSED(virtual_object);
}

static void
ray_motion(void* data, uint32_t time, vec3 origin, vec3 direction)
{
  struct zms_app* app = data;
  Z_UNUSED(app);
  Z_UNUSED(time);
  Z_UNUSED(origin);
  Z_UNUSED(direction);
}

static void
ray_button(
    void* data, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  struct zms_app* app = data;
  Z_UNUSED(app);
  Z_UNUSED(serial);
  Z_UNUSED(time);
  Z_UNUSED(button);
  Z_UNUSED(state);
}

static const struct zms_ray_interface ray_interface = {
    .enter = ray_enter,
    .leave = ray_leave,
    .motion = ray_motion,
    .button = ray_button,
};

ZMS_EXPORT void
zms_app_seat_capabilities(void* data, uint32_t capabilities)
{
  struct zms_app* app = data;
  if (capabilities & ZGN_SEAT_CAPABILITY_RAY && app->ray == NULL) {
    app->ray = zms_ray_create(app->backend, app, &ray_interface);
  }

  if (!(capabilities & ZGN_SEAT_CAPABILITY_RAY) && app->ray) {
    zms_ray_destroy(app->ray);
    app->ray = NULL;
  }

  // TODO: notify the capability change
}
