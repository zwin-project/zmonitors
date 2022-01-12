#include "virtual-object.h"

#include <zmonitors-util.h>

#include "backend.h"
#include "frame-callback.h"

ZMS_EXPORT struct zms_virtual_object *
zms_virtual_object_create(void *user_data,
    const struct zms_virtual_object_interface *interface,
    struct zms_backend *backend)
{
  struct zms_virtual_object *virtual_object;
  struct zgn_virtual_object *proxy;

  virtual_object = zalloc(sizeof *virtual_object);
  if (virtual_object == NULL) goto err;

  proxy = zgn_compositor_create_virtual_object(backend->compositor);
  if (proxy == NULL) goto err_proxy;

  virtual_object->proxy = proxy;
  wl_proxy_set_user_data((struct wl_proxy *)proxy, virtual_object);

  virtual_object->user_data = user_data;
  virtual_object->interface = interface;
  virtual_object->backend = backend;
  zms_signal_init(&virtual_object->destroy_signal);

  return virtual_object;

err_proxy:
  free(virtual_object);

err:
  return NULL;
}

ZMS_EXPORT void
zms_virtual_object_destroy(struct zms_virtual_object *virtual_object)
{
  zms_signal_emit(&virtual_object->destroy_signal, NULL);

  zgn_virtual_object_destroy(virtual_object->proxy);
  free(virtual_object);
}
