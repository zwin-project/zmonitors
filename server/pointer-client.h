#ifndef ZMONITORS_SERVER_POINTER_CLIENT_H
#define ZMONITORS_SERVER_POINTER_CLIENT_H

#include <zmonitors-util.h>

#include "pointer.h"
#include "surface.h"

struct zms_pointer_client {
  struct zms_pointer* pointer;
  struct wl_list link;  // -> zms_pointer.pointer_client_list
  struct zms_listener pointer_destroy_listener;

  struct wl_client* client;
  struct wl_listener client_destroy_listener;

  struct wl_list resource_list;
};

struct wl_resource* zms_pointer_client_resource_create(
    struct zms_pointer_client* pointer_client, struct wl_client* client,
    uint32_t id);

struct wl_resource* zms_pointer_client_inert_resource_create(
    struct wl_client* client, uint32_t id);

struct zms_pointer_client* zms_pointer_client_find(
    struct wl_client* client, struct zms_pointer* pointer);

struct zms_pointer_client* zms_pointer_client_ensure(
    struct wl_client* client, struct zms_pointer* pointer);

void zms_pointer_client_send_leave(
    struct zms_pointer_client* pointer_client, struct zms_surface* surface);

void zms_pointer_client_send_enter(struct zms_pointer_client* pointer_client,
    uint32_t serial, struct zms_surface* surface, float vx, float vy);

void zms_pointer_client_send_motion(struct zms_pointer_client* pointer_client,
    uint32_t time, float vx, float vy);

void zms_pointer_client_send_button(struct zms_pointer_client* pointer_client,
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

void zms_pointer_client_send_axis(struct zms_pointer_client* pointer_client,
    uint32_t time, uint32_t axis, float value);

void zms_pointer_client_send_frame(struct zms_pointer_client* pointer_client);

void zms_pointer_client_send_axis_discrete(
    struct zms_pointer_client* pointer_client, uint32_t axis, int32_t discrete);

#endif  //  ZMONITORS_SERVER_POINTER_CLIENT_H
