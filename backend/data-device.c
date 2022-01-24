#include "data-device.h"

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

#include "backend.h"
#include "data-offer.h"
#include "data-source.h"
#include "virtual-object.h"

static void
zms_backend_data_device_protocol_data_offer(void* data,
    struct zgn_data_device* zgn_data_device, struct zgn_data_offer* offer_proxy)
{
  Z_UNUSED(zgn_data_device);
  struct zms_backend_data_device* device = data;
  struct zms_backend_data_offer* offer;

  offer = zms_backend_data_offer_create(offer_proxy);

  device->backend->interface->data_offer(device->backend->user_data, offer);
}

static void
zms_backend_data_device_protocol_enter(void* data,
    struct zgn_data_device* zgn_data_device, uint32_t serial,
    struct zgn_virtual_object* virtual_object_proxy, struct wl_array* origin,
    struct wl_array* direction, struct zgn_data_offer* data_offer_proxy)
{
  Z_UNUSED(zgn_data_device);
  struct zms_backend_data_device* data_device = data;
  struct zms_virtual_object* virtual_object;
  struct zms_backend_data_offer* data_offer =
      wl_proxy_get_user_data((struct wl_proxy*)data_offer_proxy);
  vec3 origin_vec, direction_vec;

  virtual_object =
      wl_proxy_get_user_data((struct wl_proxy*)virtual_object_proxy);

  zms_weak_reference(&data_device->focus_virtual_object_ref, virtual_object,
      &virtual_object->destroy_signal);

  glm_vec3_from_wl_array(origin_vec, origin);
  glm_vec3_from_wl_array(direction_vec, direction);
  virtual_object->interface->data_device_enter(virtual_object->user_data,
      serial, origin_vec, direction_vec, data_offer->user_data);
}

static void
zms_backend_data_device_protocol_leave(
    void* data, struct zgn_data_device* zgn_data_device)
{
  Z_UNUSED(zgn_data_device);
  struct zms_backend_data_device* data_device = data;
  struct zms_virtual_object* focus_virtual_object;

  focus_virtual_object = data_device->focus_virtual_object_ref.data;

  zms_weak_reference(&data_device->focus_virtual_object_ref, NULL, NULL);
  focus_virtual_object->interface->data_device_leave(
      focus_virtual_object->user_data);
}

static void
zms_backend_data_device_protocol_motion(void* data,
    struct zgn_data_device* zgn_data_device, uint32_t time,
    struct wl_array* origin, struct wl_array* direction)
{
  Z_UNUSED(zgn_data_device);
  struct zms_backend_data_device* data_device = data;
  struct zms_virtual_object* focus_virtual_object;
  vec3 origin_vec, direction_vec;

  focus_virtual_object = data_device->focus_virtual_object_ref.data;

  if (focus_virtual_object == NULL) return;

  glm_vec3_from_wl_array(origin_vec, origin);
  glm_vec3_from_wl_array(direction_vec, direction);
  focus_virtual_object->interface->data_device_motion_abs(
      focus_virtual_object->user_data, time, origin_vec, direction_vec);
}

static void
zms_backend_data_device_protocol_drop(
    void* data, struct zgn_data_device* zgn_data_device)
{
  Z_UNUSED(zgn_data_device);
  struct zms_backend_data_device* data_device = data;
  struct zms_virtual_object* focus_virtual_object;

  focus_virtual_object = data_device->focus_virtual_object_ref.data;

  if (focus_virtual_object == NULL) return;

  focus_virtual_object->interface->data_device_drop(
      focus_virtual_object->user_data);
}

static const struct zgn_data_device_listener data_device_listener = {
    .data_offer = zms_backend_data_device_protocol_data_offer,
    .enter = zms_backend_data_device_protocol_enter,
    .leave = zms_backend_data_device_protocol_leave,
    .motion = zms_backend_data_device_protocol_motion,
    .drop = zms_backend_data_device_protocol_drop,
};

ZMS_EXPORT void
zms_backend_data_device_start_drag(struct zms_backend* backend,
    struct zms_backend_data_source* data_source,
    struct zms_virtual_object* virtual_object,
    struct zms_virtual_object* icon /* nullable */, uint32_t serial)
{
  zgn_data_device_start_drag(backend->data_device->proxy, data_source->proxy,
      virtual_object->proxy, icon ? icon->proxy : NULL, serial);
}

ZMS_EXPORT struct zms_backend_data_device*
zms_backend_data_device_create(struct zms_backend* backend)
{
  struct zms_backend_data_device* data_device;
  struct zgn_data_device* proxy;

  data_device = zalloc(sizeof *data_device);
  if (data_device == NULL) goto err;

  proxy = zgn_data_device_manager_get_data_device(
      backend->data_device_manager, backend->seat);
  zgn_data_device_add_listener(proxy, &data_device_listener, data_device);

  data_device->proxy = proxy;
  data_device->backend = backend;
  zms_weak_ref_init(&data_device->focus_virtual_object_ref);

  return data_device;

err:
  return NULL;
}

ZMS_EXPORT void
zms_backend_data_device_destroy(struct zms_backend_data_device* data_device)
{
  zms_weak_reference(&data_device->focus_virtual_object_ref, NULL, NULL);
  zgn_data_device_destroy(data_device->proxy);
  free(data_device);
}
