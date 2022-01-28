#ifndef ZMONITORS_BACKEND_KEYBOARD_H
#define ZMONITORS_BACKEND_KEYBOARD_H

#include <zigen-client-protocol.h>
#include <zmonitors-backend.h>

struct zms_backend_keyboard {
  struct zgn_keyboard *proxy;
  struct zms_weak_ref focus_virtual_object_ref;

  struct zms_backend *backend;
};

struct zms_backend_keyboard *zms_backend_keyboard_create(
    struct zms_backend *backend);

void zms_backend_keyboard_destroy(struct zms_backend_keyboard *keyboard);

#endif  //  ZMONITORS_BACKEND_KEYBOARD_H
