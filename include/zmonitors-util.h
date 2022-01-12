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

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static inline void *
zalloc(size_t size)
{
  return calloc(1, size);
}

int zms_util_create_shared_fd(size_t size, const char *name);

int glm_vec3_from_wl_array(vec3 v, struct wl_array *array);

void glm_vec3_to_wl_array(vec3 v, struct wl_array *array);

int glm_versor_from_wl_array(versor v, struct wl_array *array);

void glm_versor_to_wl_array(versor v, struct wl_array *array);

int glm_mat4_from_wl_array(mat4 m, struct wl_array *array);

void glm_mat4_to_wl_array(mat4 m, struct wl_array *array);

int zms_log(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

struct zms_listener;

typedef void (*zms_notify_func_t)(struct zms_listener *listener, void *data);

struct zms_listener {
  struct wl_list link;
  zms_notify_func_t notify;
};

struct zms_signal {
  struct wl_list listener_list;
};

static inline void
zms_signal_init(struct zms_signal *signal)
{
  wl_list_init(&signal->listener_list);
}

static inline void
zms_signal_add(struct zms_signal *signal, struct zms_listener *listener)
{
  wl_list_insert(signal->listener_list.prev, &listener->link);
}

static inline struct zms_listener *
zms_signal_get(struct zms_signal *signal, zms_notify_func_t notify)
{
  struct zms_listener *l;

  wl_list_for_each(
      l, &signal->listener_list, link) if (l->notify == notify) return l;

  return NULL;
}

static inline void
zms_signal_emit(struct zms_signal *signal, void *data)
{
  struct zms_listener *l, *next;

  wl_list_for_each_safe(l, next, &signal->listener_list, link)
      l->notify(l, data);
}

#ifdef __cplusplus
}
#endif

#endif  //  ZMONITORS_UTIL_H
