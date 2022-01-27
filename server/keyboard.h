#ifndef ZMONITORS_SERVER_KEYBOARD_H
#define ZMONITORS_SERVER_KEYBOARD_H

#include <zmonitors-server.h>

struct zms_keyboard {
  struct zms_seat *seat; /* nonnull */

  struct wl_list keyboard_client_list;

  int keymap_fd; /* zms_keyboard manages this fd */
  uint32_t keymap_size;
  uint32_t keymap_format;

  struct zms_weak_ref focus_view_ref;

  // signals
  struct zms_signal destroy_signal;
};

struct zms_keyboard *zms_keyboard_create(struct zms_seat *seat);

void zms_keyboard_destroy(struct zms_keyboard *keyboard);

void zms_keyboard_set_keymap(
    struct zms_keyboard *keyboard, int fd, uint32_t size, uint32_t format);

void zms_keyboard_set_focus(
    struct zms_keyboard *keyboard, struct zms_view *view /* nullable */);

void zms_keyboard_send_key(struct zms_keyboard *keyboard, uint32_t serial,
    uint32_t time, uint32_t key, uint32_t state);

void zms_keyboard_send_modifiers(struct zms_keyboard *keyboard, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
    uint32_t group);

void zms_keyboard_send_keymap_to_resource(
    struct zms_keyboard *keyboard, struct wl_resource *keyboard_resource);

#endif  //  ZMONITORS_SERVER_KEYBOARD_H
