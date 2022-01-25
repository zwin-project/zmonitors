#ifndef ZMONITORS_SERVER_DATA_DEVICE_H
#define ZMONITORS_SERVER_DATA_DEVICE_H

#include <wayland-server.h>
#include <zmonitors-util.h>

#include "view.h"

struct zms_data_device {
  struct zms_seat* seat;
  struct wl_list resource_list;

  struct zms_data_offer_proxy* data_offer_proxy;
  struct zms_listener data_offer_proxy_destroy_listener;
  struct zms_listener data_offer_proxy_action_change_listener;
  struct zms_listener data_offer_proxy_source_actions_change_listener;

  struct zms_weak_ref data_offer_in_use_ref;

  struct zms_weak_ref focus_view_ref;
  float vx, vy;
  uint32_t focus_serial;

  uint32_t proxy_enter_serial;
  bool is_dragging;
};

struct zms_data_device* zms_data_device_create(struct zms_seat* seat);

void zms_data_device_destroy(struct zms_data_device* data_device);

struct wl_resource* zms_data_device_create_resource(
    struct zms_data_device* data_device, struct wl_client* client, uint32_t id);

enum zms_data_device_set_focus_result {
  ZMS_DATA_DEVICE_SET_FOCUS_LEFT = 1,
  ZMS_DATA_DEVICE_SET_FOCUS_ENTERED = 2,
};

enum zms_data_device_set_focus_result zms_data_device_set_focus(
    struct zms_data_device* data_device, struct zms_view* view /* nullable */,
    float vx, float vy);

void zms_data_device_send_motion(
    struct zms_data_device* data_device, uint32_t time);

#endif  //  ZMONITORS_SERVER_DATA_DEVICE_H
