#ifndef ZMONITORS_UTIL_H
#define ZMONITORS_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cglm/cglm.h>
#include <stdlib.h>
#include <wayland-util.h>
#include <zmonitors-types.h>

/** Visibility attribute */
#if defined(__GNUC__) && __GNUC__ >= 4
#define ZMS_EXPORT __attribute__((visibility("default")))
#else
#define ZMS_EXPORT
#endif

/** Deprecated attribute */
#if defined(__GNUC__) && __GNUC__ >= 4
#define ZMS_DEPRECATED __attribute__((deprecated))
#else
#define ZMS_DEPRECATED
#endif

#ifndef Z_UNUSED
#define Z_UNUSED(x) ((void)x)
#endif

static inline void *
zalloc(size_t size)
{
  return calloc(1, size);
}

int glm_vec3_from_wl_array(vec3 v, struct wl_array *array);

void glm_vec3_to_wl_array(vec3 v, struct wl_array *array);

int glm_versor_from_wl_array(versor v, struct wl_array *array);

void glm_versor_to_wl_array(versor v, struct wl_array *array);

int zms_log(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#ifdef __cplusplus
}
#endif

#endif  //  ZMONITORS_UTIL_H
