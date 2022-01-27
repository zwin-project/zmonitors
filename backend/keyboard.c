#include "keyboard.h"

#include <linux/input.h>
#include <unistd.h>

#include "backend.h"
#include "virtual-object.h"

static void
zms_backend_keyboard_protocol_keymap(void *data,
    struct zgn_keyboard *zgn_keyboard, uint32_t format, int32_t fd,
    uint32_t size)
{
  Z_UNUSED(zgn_keyboard);
  struct zms_backend_keyboard *keyboard = data;

  keyboard->backend->interface->keyboard_keymap(
      keyboard->backend->user_data, format, fd, size);
}

static void
zms_backend_keyboard_protocol_enter(void *data,
    struct zgn_keyboard *keyboard_proxy, uint32_t serial,
    struct zgn_virtual_object *virtual_object_proxy, struct wl_array *keys)
{
  Z_UNUSED(keyboard_proxy);
  struct zms_backend_keyboard *keyboard = data;
  struct zms_virtual_object *virtual_object;

  virtual_object =
      wl_proxy_get_user_data((struct wl_proxy *)virtual_object_proxy);

  zms_weak_reference(&keyboard->focus_virtual_object_ref, virtual_object,
      &virtual_object->destroy_signal);

  virtual_object->interface->keyboard_enter(
      virtual_object->user_data, serial, keys);
}

static void
zms_backend_keyboard_protocol_leave(void *data,
    struct zgn_keyboard *zgn_keyboard, uint32_t serial,
    struct zgn_virtual_object *virtual_object_proxy)
{
  Z_UNUSED(zgn_keyboard);
  Z_UNUSED(virtual_object_proxy);
  struct zms_backend_keyboard *keyboard = data;
  struct zms_virtual_object *virtual_object =
      keyboard->focus_virtual_object_ref.data;

  zms_weak_reference(&keyboard->focus_virtual_object_ref, NULL, NULL);

  if (virtual_object == NULL) return;

  virtual_object->interface->keyboard_leave(virtual_object->user_data, serial);
}

static void
zms_backend_keyboard_protocol_key(void *data, struct zgn_keyboard *zgn_keyboard,
    uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
  Z_UNUSED(zgn_keyboard);
  struct zms_backend_keyboard *keyboard = data;
  struct zms_virtual_object *virtual_object =
      keyboard->focus_virtual_object_ref.data;

  if (virtual_object == NULL) return;

  virtual_object->interface->keyboard_key(
      virtual_object->user_data, serial, time, key, state);
}

static void
zms_backend_keyboard_protocol_modifiers(void *data,
    struct zgn_keyboard *zgn_keyboard, uint32_t serial, uint32_t mods_depressed,
    uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
  Z_UNUSED(zgn_keyboard);

  struct zms_backend_keyboard *keyboard = data;
  struct zms_virtual_object *virtual_object =
      keyboard->focus_virtual_object_ref.data;

  if (virtual_object == NULL) return;

  virtual_object->interface->keyboard_modifiers(virtual_object->user_data,
      serial, mods_depressed, mods_latched, mods_locked, group);
}

static const struct zgn_keyboard_listener keyboard_listener = {
    .keymap = zms_backend_keyboard_protocol_keymap,
    .enter = zms_backend_keyboard_protocol_enter,
    .leave = zms_backend_keyboard_protocol_leave,
    .key = zms_backend_keyboard_protocol_key,
    .modifiers = zms_backend_keyboard_protocol_modifiers,
};

struct zms_backend_keyboard *
zms_backend_keyboard_create(struct zms_backend *backend)
{
  struct zms_backend_keyboard *keyboard;
  struct zgn_keyboard *proxy;

  keyboard = zalloc(sizeof *keyboard);
  if (keyboard == NULL) goto err;

  proxy = zgn_seat_get_keyboard(backend->seat);
  if (proxy == NULL) goto err_proxy;

  zgn_keyboard_add_listener(proxy, &keyboard_listener, keyboard);

  keyboard->proxy = proxy;
  zms_weak_ref_init(&keyboard->focus_virtual_object_ref);
  keyboard->backend = backend;

  return keyboard;

err_proxy:
  free(keyboard);

err:
  return NULL;
}

void
zms_backend_keyboard_destroy(struct zms_backend_keyboard *keyboard)
{
  zms_weak_reference(&keyboard->focus_virtual_object_ref, NULL, NULL);
  zgn_keyboard_destroy(keyboard->proxy);
  free(keyboard);
}
