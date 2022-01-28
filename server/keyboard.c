#include "keyboard.h"

#include <unistd.h>

#include "keyboard-client.h"
#include "seat.h"
#include "view.h"

ZMS_EXPORT struct zms_keyboard *
zms_keyboard_create(struct zms_seat *seat)
{
  struct zms_keyboard *keyboard;

  keyboard = zalloc(sizeof *keyboard);
  if (keyboard == NULL) goto err;

  keyboard->seat = seat;
  wl_list_init(&keyboard->keyboard_client_list);
  zms_weak_ref_init(&keyboard->focus_view_ref);
  zms_signal_init(&keyboard->destroy_signal);

  return keyboard;

err:
  return NULL;
}

ZMS_EXPORT void
zms_keyboard_destroy(struct zms_keyboard *keyboard)
{
  if (keyboard->keymap_fd > 0) close(keyboard->keymap_fd);
  zms_signal_emit(&keyboard->destroy_signal, NULL);
  zms_weak_reference(&keyboard->focus_view_ref, NULL, NULL);
  free(keyboard);
}

ZMS_EXPORT void
zms_keyboard_set_keymap(
    struct zms_keyboard *keyboard, int fd, uint32_t size, uint32_t format)
{
  if (keyboard->keymap_fd > 0) close(keyboard->keymap_fd);
  keyboard->keymap_fd = fd;
  keyboard->keymap_size = size;
  keyboard->keymap_format = format;
  // TODO: notify the change
}

ZMS_EXPORT void
zms_keyboard_set_focus(
    struct zms_keyboard *keyboard, struct zms_view *view /* nullable */)
{
  struct zms_view *prev_focus = keyboard->focus_view_ref.data;
  struct zms_keyboard_client *keyboard_client;
  struct wl_client *client;
  struct wl_display *display = keyboard->seat->priv->compositor->display;

  if (prev_focus) {
    uint32_t serial = wl_display_next_serial(display);
    client = wl_resource_get_client(prev_focus->priv->surface->resource);
    keyboard_client = zms_keyboard_client_find(client, keyboard);
    if (keyboard_client)
      zms_keyboard_client_send_leave(keyboard_client, serial, prev_focus);
  }

  if (view) {
    uint32_t serial = wl_display_next_serial(display);
    struct wl_array keys;  // TODO: set keys pressed
    wl_array_init(&keys);
    client = wl_resource_get_client(view->priv->surface->resource);
    keyboard_client = zms_keyboard_client_find(client, keyboard);
    if (keyboard_client)
      zms_keyboard_client_send_enter(keyboard_client, serial, view, &keys);
    wl_array_release(&keys);
  }

  zms_weak_reference(
      &keyboard->focus_view_ref, view, view ? &view->destroy_signal : NULL);
}

ZMS_EXPORT void
zms_keyboard_send_key(struct zms_keyboard *keyboard, uint32_t serial,
    uint32_t time, uint32_t key, uint32_t state)
{
  struct zms_view *view = keyboard->focus_view_ref.data;
  struct zms_keyboard_client *keyboard_client;
  struct wl_client *client;

  if (view == NULL) return;

  client = wl_resource_get_client(view->priv->surface->resource);

  keyboard_client = zms_keyboard_client_find(client, keyboard);

  if (keyboard_client == NULL) return;

  zms_keyboard_client_send_key(keyboard_client, serial, time, key, state);
}

ZMS_EXPORT void
zms_keyboard_send_modifiers(struct zms_keyboard *keyboard, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
    uint32_t group)
{
  struct zms_view *view = keyboard->focus_view_ref.data;
  struct zms_keyboard_client *keyboard_client;
  struct wl_client *client;

  if (view == NULL) return;

  client = wl_resource_get_client(view->priv->surface->resource);

  keyboard_client = zms_keyboard_client_find(client, keyboard);

  if (keyboard_client == NULL) return;

  zms_keyboard_client_send_modifiers(keyboard_client, serial, mods_depressed,
      mods_latched, mods_locked, group);
}

ZMS_EXPORT void
zms_keyboard_send_keymap_to_resource(
    struct zms_keyboard *keyboard, struct wl_resource *keyboard_resource)
{
  if (keyboard->keymap_fd <= 0) return;
  wl_keyboard_send_keymap(keyboard_resource, keyboard->keymap_format,
      keyboard->keymap_fd, keyboard->keymap_size);
}
