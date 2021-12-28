#include <string.h>
#include <wayland-client.h>
#include <zigen-client-protocol.h>
#include <zigen-opengl-client-protocol.h>
#include <zigen-shell-client-protocol.h>
#include <zmonitors-util.h>

#include "zmonitors-backend.h"

struct zms_backend {
  struct wl_display* display;

  /* globals */
  struct zgn_compositor* compositor;
  struct zgn_seat* seat;
  struct zgn_shell* shell;
  struct wl_shm* shm;
  struct zgn_opengl* opengl;
};

static void
seat_capabilities(void* data, struct zgn_seat* seat, uint32_t capability)
{
  // TODO:
  zms_log("event not implemented yet: zgn_seat.capabilities\n");
  Z_UNUSED(data);
  Z_UNUSED(seat);
  Z_UNUSED(capability);
}

static const struct zgn_seat_listener seat_listener = {
    .capabilities = seat_capabilities,
};

static void
shm_format(void* data, struct wl_shm* shm, uint32_t format)
{
  // we use only the default formats (0 and 1).
  Z_UNUSED(data);
  Z_UNUSED(shm);
  Z_UNUSED(format);
}

static const struct wl_shm_listener shm_listener = {
    .format = shm_format,
};

static void
global_registry_handler(void* data, struct wl_registry* registry, uint32_t id,
    const char* interface, uint32_t version)
{
  struct zms_backend* backend = data;

  if (strcmp(interface, "zgn_compositor") == 0) {
    backend->compositor =
        wl_registry_bind(registry, id, &zgn_compositor_interface, version);
  } else if (strcmp(interface, "zgn_seat") == 0) {
    backend->seat =
        wl_registry_bind(registry, id, &zgn_seat_interface, version);
    zgn_seat_add_listener(backend->seat, &seat_listener, backend);
  } else if (strcmp(interface, "zgn_shell") == 0) {
    backend->shell =
        wl_registry_bind(registry, id, &zgn_shell_interface, version);
  } else if (strcmp(interface, "wl_shm") == 0) {
    backend->shm = wl_registry_bind(registry, id, &wl_shm_interface, version);
    wl_shm_add_listener(backend->shm, &shm_listener, backend);
  } else if (strcmp(interface, "zgn_opengl") == 0) {
    backend->opengl =
        wl_registry_bind(registry, id, &zgn_opengl_interface, version);
  }
}

static void
global_registry_remover(void* data, struct wl_registry* registry, uint32_t id)
{
  // TODO:
  zms_log("event not implemented yet: wl_registry.global_remove\n");
  Z_UNUSED(data);
  Z_UNUSED(registry);
  Z_UNUSED(id);
}

static const struct wl_registry_listener registry_listener = {
    .global = global_registry_handler,
    .global_remove = global_registry_remover,
};

ZMS_EXPORT struct zms_backend*
zms_backend_create()
{
  struct zms_backend* backend;

  backend = zalloc(sizeof *backend);
  if (backend == NULL) {
    zms_log("failed to allocate memory\n");
    goto err;
  }

  backend->display = NULL;

  return backend;

err:
  return NULL;
}

ZMS_EXPORT void
zms_backend_destroy(struct zms_backend* backend)
{
  if (backend->display) wl_display_disconnect(backend->display);
  free(backend);
}

ZMS_EXPORT bool
zms_backend_connect(struct zms_backend* backend, const char* socket)
{
  struct wl_display* display;
  struct wl_registry* registry;

  display = wl_display_connect(socket);
  if (display == NULL) goto err;

  registry = wl_display_get_registry(display);
  if (registry == NULL) goto err_registry;

  wl_registry_add_listener(registry, &registry_listener, backend);

  backend->display = display;
  wl_display_dispatch(display);
  wl_display_roundtrip(display);

  if (backend->compositor == NULL || backend->seat == NULL ||
      backend->shell == NULL || backend->shm == NULL || backend->opengl == NULL)
    goto err_globals;

  return true;

err_globals:
  wl_registry_destroy(registry);

err_registry:
  wl_display_disconnect(display);
  backend->display = NULL;

err:
  return false;
}

ZMS_EXPORT int
zms_backend_get_fd(struct zms_backend* backend)
{
  return wl_display_get_fd(backend->display);
}

ZMS_EXPORT int
zms_backend_dispatch(struct zms_backend* backend)
{
  return wl_display_dispatch(backend->display);
}

ZMS_EXPORT int
zms_backend_flush(struct zms_backend* backend)
{
  return wl_display_flush(backend->display);
}

ZMS_EXPORT int
zms_backend_dispatch_pending(struct zms_backend* backend)
{
  return wl_display_dispatch_pending(backend->display);
}
