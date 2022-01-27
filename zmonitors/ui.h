#ifndef ZMONITORS_UI_H
#define ZMONITORS_UI_H

#include <zmonitors-backend.h>

/* ui base */

struct zms_ui_base;

struct zms_ui_base_interface {
  void (*setup)(struct zms_ui_base* ui_base);                /* nonnull */
  void (*teardown)(struct zms_ui_base* ui_base);             /* nonnull */
  void (*reconfigure)(struct zms_ui_base* ui_base);          /* nonnull */
  void (*repaint)(struct zms_ui_base* ui_base);              /* nullable */
  void (*frame)(struct zms_ui_base* ui_base, uint32_t time); /* nullable */
  bool (*ray_enter)(struct zms_ui_base* ui_base, uint32_t serial, vec3 origin,
      vec3 direction); /* nullable */
  bool (*ray_leave)(
      struct zms_ui_base* ui_base, uint32_t serial); /* nullable */
  bool (*ray_motion)(struct zms_ui_base* ui_base, uint32_t time, vec3 origin,
      vec3 direction); /* nullable */
  bool (*ray_button)(struct zms_ui_base* ui_base, uint32_t serial,
      uint32_t time, uint32_t button, uint32_t state); /* nullable */
  bool (*keyboard_enter)(struct zms_ui_base* ui_base, uint32_t serial,
      struct wl_array* keys); /* nullable */
  bool (*keyboard_leave)(
      struct zms_ui_base* ui_base, uint32_t serial); /* nullable */
  bool (*keyboard_key)(struct zms_ui_base* ui_base, uint32_t serial,
      uint32_t time, uint32_t key, uint32_t state); /* nullable */
  bool (*keyboard_modifiers)(struct zms_ui_base* ui_base, uint32_t serial,
      uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
      uint32_t group); /* nullable */
  bool (*cuboid_window_moved)(
      struct zms_ui_base* ui_base, vec3 face_direction); /* nullable */
};

struct zms_ui_base {
  void* user_data;
  const struct zms_ui_base_interface* interface;

  struct zms_ui_root* root;
  struct zms_ui_base* parent;
  struct wl_list link;  // -> zms_ui_base.children
  struct wl_list children;

  vec3 position;
  vec3 half_size;

  bool setup;
};

struct zms_ui_base* zms_ui_base_create(void* user_data,
    const struct zms_ui_base_interface* interface, struct zms_ui_base* parent);

void zms_ui_base_destroy(struct zms_ui_base* ui_base);

void zms_ui_base_schedule_repaint(struct zms_ui_base* ui_base);

/* root */

struct zms_ui_root {
  struct zms_ui_base* base;
  struct zms_cuboid_window* cuboid_window;
  struct wl_list frame_callback_list;
  uint32_t frame_state;  // enum zms_ui_frame_state
};

struct zms_ui_root* zms_ui_root_create(void* user_data,
    const struct zms_ui_base_interface* interface, struct zms_backend* backend,
    vec3 half_size, versor quaternion);

void zms_ui_root_destroy(struct zms_ui_root* root);

#endif  //  ZMONITORS_UI_H
