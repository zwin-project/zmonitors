#include <zmonitors-util.h>

static void
zms_weak_ref_destroy_handler(struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_weak_ref *ref = wl_container_of(listener, ref, destroy_listener);
  wl_list_remove(&ref->destroy_listener.link);
  ref->data = NULL;
}

ZMS_EXPORT void
zms_weak_ref_init(struct zms_weak_ref *ref)
{
  ref->data = NULL;
}

ZMS_EXPORT void
zms_weak_reference(
    struct zms_weak_ref *ref, void *data, struct zms_signal *destroy_signal)
{
  if (ref->data) wl_list_remove(&ref->destroy_listener.link);
  if (data) zms_signal_add(destroy_signal, &ref->destroy_listener);

  ref->data = data;
  ref->destroy_listener.notify = zms_weak_ref_destroy_handler;
}
