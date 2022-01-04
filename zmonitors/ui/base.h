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

#endif  //  ZMONITORS_UI_BASE_H
