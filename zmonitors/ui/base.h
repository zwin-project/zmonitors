#ifndef ZMONITORS_UI_BASE_H
#define ZMONITORS_UI_BASE_H

#include <wayland-util.h>
#include <zmonitors-backend.h>

#include "ui.h"

struct zms_ui_base* zms_ui_base_create_root(struct zms_ui_root* root,
    void* user_data, const struct zms_ui_base_interface* interface);

void zms_ui_base_run_setup_phase(struct zms_ui_base* ui_base);

void zms_ui_base_run_repaint_phase(struct zms_ui_base* ui_base);

void zms_ui_base_run_frame_phase(struct zms_ui_base* ui_base, uint32_t time);

void zms_ui_base_run_reconfigure_phase(struct zms_ui_base* ui_base);

bool zms_ui_base_propagate_ray_enter(
    struct zms_ui_base* ui_base, uint32_t serial, vec3 origin, vec3 direction);

bool zms_ui_base_propagate_ray_leave(
    struct zms_ui_base* ui_base, uint32_t serial);

bool zms_ui_base_propagate_ray_motion(
    struct zms_ui_base* ui_base, uint32_t time, vec3 origin, vec3 direction);

bool zms_ui_base_propagate_ray_button(struct zms_ui_base* ui_base,
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

bool zms_ui_base_propagate_cuboid_window_moved(
    struct zms_ui_base* ui_base, vec3 face_direction);

#endif  //  ZMONITORS_UI_BASE_H
