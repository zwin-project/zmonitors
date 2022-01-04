#include "frame-callback.h"

#include <zigen-client-protocol.h>
#include <zmonitors-util.h>

#include "virtual-object.h"
#include "zmonitors-backend.h"

static void
frame_callback_done(
    void* data, struct wl_callback* callback, uint32_t callback_data)
{
  Z_UNUSED(callback);
  struct zms_frame_callback* frame_callback = data;
  frame_callback->priv->user_func(
      frame_callback->priv->user_data, callback_data);
  zms_frame_callback_destroy(frame_callback);
}

static const struct wl_callback_listener frame_callback_listener = {
    .done = frame_callback_done,
};

static void
virtual_object_destroy_handler(struct zms_listener* listener, void* data)
{
  Z_UNUSED(data);
  struct zms_frame_callback_private* frame_callback_priv;

  frame_callback_priv = wl_container_of(
      listener, frame_callback_priv, virtual_object_destroy_listener);

  zms_frame_callback_destroy(frame_callback_priv->pub);
}

ZMS_EXPORT struct zms_frame_callback*
zms_frame_callback_create(struct zms_virtual_object* virtual_object, void* data,
    zms_frame_callback_func_t callback_func)
{
  struct zms_frame_callback* frame_callback;
  struct zms_frame_callback_private* priv;
  struct wl_callback* callback;

  frame_callback = zalloc(sizeof *frame_callback);
  if (frame_callback == NULL) goto err;

  priv = zalloc(sizeof *priv);
  if (priv == NULL) goto err_priv;

  callback = zgn_virtual_object_frame(virtual_object->proxy);
  if (callback == NULL) goto err_callback;
  wl_callback_add_listener(callback, &frame_callback_listener, frame_callback);

  priv->pub = frame_callback;
  priv->user_data = data;
  priv->user_func = callback_func;
  priv->callback = callback;
  priv->virtual_object_destroy_listener.notify = virtual_object_destroy_handler;
  zms_signal_add(
      &virtual_object->destroy_signal, &priv->virtual_object_destroy_listener);
  frame_callback->priv = priv;
  wl_list_init(&frame_callback->link);

  return frame_callback;

err_callback:
  free(priv);

err_priv:
  free(frame_callback);

err:
  return NULL;
}

ZMS_EXPORT void
zms_frame_callback_destroy(struct zms_frame_callback* frame_callback)
{
  wl_list_remove(&frame_callback->link);
  wl_list_remove(&frame_callback->priv->virtual_object_destroy_listener.link);
  wl_callback_destroy(frame_callback->priv->callback);
  free(frame_callback->priv);
  free(frame_callback);
}
