#ifndef ZMONITORS_SERVER_KEYBOARD_CLIENT_H
#define ZMONITORS_SERVER_KEYBOARD_CLIENT_H

#include <zmonitors-server.h>

#include "keyboard.h"

struct zms_keyboard_client {
  struct zms_keyboard *keyboard;
  struct wl_list link;  // ->zms_keyboard.keyboard_client_list
  struct zms_listener keyboard_destroy_listener;

  struct wl_client *client;
  struct wl_listener client_destroy_listener;

  struct wl_list resource_list;
};

struct wl_resource *zms_keyboard_client_resource_create(
    struct zms_keyboard_client *keyboard_client, struct wl_client *client,
    uint32_t id);

struct wl_resource *zms_keyboard_client_inert_resource_create(
    struct wl_client *client, uint32_t id);

struct zms_keyboard_client *zms_keyboard_client_find(
    struct wl_client *client, struct zms_keyboard *keyboard);

struct zms_keyboard_client *zms_keyboard_client_ensure(
    struct wl_client *client, struct zms_keyboard *keyboard);

void zms_keyboard_client_send_enter(struct zms_keyboard_client *keyboard_client,
    uint32_t serial, struct zms_view *view, struct wl_array *keys);

void zms_keyboard_client_send_leave(struct zms_keyboard_client *keyboard_client,
    uint32_t serial, struct zms_view *view);

void zms_keyboard_client_send_key(struct zms_keyboard_client *keyboard_client,
    uint32_t serial, uint32_t time, uint32_t key, uint32_t state);

void zms_keyboard_client_send_modifiers(
    struct zms_keyboard_client *keyboard_client, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
    uint32_t group);

#endif  //  ZMONITORS_SERVER_KEYBOARD_CLIENT_H
