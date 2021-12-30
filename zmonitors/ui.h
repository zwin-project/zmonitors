#ifndef ZMONITORS_UI_H
#define ZMONITORS_UI_H

#include <zmonitors-backend.h>

/* ui base */

struct zms_ui_base_interface {
  void (*setup)(void* user_data);
  void (*teardown)(void* user_data);
};

struct zms_ui_base;

struct zms_ui_base* zms_ui_base_create(void* user_data,
    const struct zms_ui_base_interface* interface, struct zms_ui_base* parent);

void zms_ui_base_destroy(struct zms_ui_base* ui_base);

struct zms_opengl_component* zms_ui_base_get_component(
    struct zms_ui_base* base);

/* root */

struct zms_ui_root;

struct zms_ui_root* zms_ui_root_create(
    struct zms_backend* backend, vec3 half_size, versor quaternion);

void zms_ui_root_destroy(struct zms_ui_root* root);

struct zms_ui_base* zms_ui_root_get_base(struct zms_ui_root* root);

#endif  //  ZMONITORS_UI_H
