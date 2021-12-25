#ifndef ZMONITORS_UTIL_H
#define ZMONITORS_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

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

static inline void*
zalloc(size_t size)
{
  return calloc(1, size);
}

int zms_log(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

/* zms_list */

struct zms_list {
  struct zms_list* prev;
  struct zms_list* next;
};

void zms_list_init(struct zms_list* list);

void zms_list_insert(struct zms_list* list, struct zms_list* elm);

void zms_list_remove(struct zms_list* elm);

#define zms_container_of(ptr, sample, member) \
  (__typeof__(sample))((char*)(ptr)-offsetof(__typeof__(*sample), member))

#define zms_list_for_each(pos, head, member)              \
  for (pos = zms_container_of((head)->next, pos, member); \
       &pos->member != (head);                            \
       pos = zms_container_of(pos->member.next, pos, member))

#define zms_list_for_each_safe(pos, tmp, head, member)         \
  for (pos = zms_container_of((head)->next, pos, member),      \
      tmp = zms_container_of((pos)->member.next, tmp, member); \
       &pos->member != (head);                                 \
       pos = tmp, tmp = zms_container_of(pos->member.next, tmp, member))

#ifdef __cplusplus
}
#endif

#endif  //  ZMONITORS_UTIL_H
