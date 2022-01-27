#include "keyboard-client.h"

#include "view.h"

static void zms_keyboard_client_destroy(
    struct zms_keyboard_client *keyboard_client);

static void
zms_keyboard_client_handle_destroy(struct wl_resource *resource)
{
  wl_list_remove(wl_resource_get_link(resource));
}

static void
zms_keyboard_protocol_release(
    struct wl_client *client, struct wl_resource *resource)
{
  Z_UNUSED(client);

  wl_resource_destroy(resource);
}

static const struct wl_keyboard_interface keyboard_interface = {
    .release = zms_keyboard_protocol_release,
};

static void
keyboard_destroy_handler(struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_keyboard_client *keyboard_client;

  keyboard_client =
      wl_container_of(listener, keyboard_client, keyboard_destroy_listener);

  zms_keyboard_client_destroy(keyboard_client);
}

static void
client_destroy_handler(struct wl_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_keyboard_client *keyboard_client;

  keyboard_client =
      wl_container_of(listener, keyboard_client, client_destroy_listener);

  zms_keyboard_client_destroy(keyboard_client);
}

static struct zms_keyboard_client *
zms_keyboard_client_create(
    struct wl_client *client, struct zms_keyboard *keyboard)
{
  struct zms_keyboard_client *keyboard_client;

  keyboard_client = zalloc(sizeof *keyboard_client);
  if (keyboard_client == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  keyboard_client->keyboard = keyboard;
  wl_list_insert(&keyboard->keyboard_client_list, &keyboard_client->link);

  keyboard_client->keyboard_destroy_listener.notify = keyboard_destroy_handler;
  zms_signal_add(
      &keyboard->destroy_signal, &keyboard_client->keyboard_destroy_listener);

  keyboard_client->client = client;
  keyboard_client->client_destroy_listener.notify = client_destroy_handler;
  wl_client_add_destroy_listener(
      client, &keyboard_client->client_destroy_listener);

  wl_list_init(&keyboard_client->resource_list);

  return keyboard_client;

err:
  return NULL;
}

static void
zms_keyboard_client_destroy(struct zms_keyboard_client *keyboard_client)
{
  struct wl_resource *resource, *tmp;

  // make the resources inert
  wl_resource_for_each_safe(resource, tmp, &keyboard_client->resource_list)
  {
    wl_resource_set_destructor(resource, NULL);
    wl_resource_set_user_data(resource, NULL);
    wl_list_remove(wl_resource_get_link(resource));
  }

  wl_list_remove(&keyboard_client->link);
  wl_list_remove(&keyboard_client->keyboard_destroy_listener.link);
  wl_list_remove(&keyboard_client->client_destroy_listener.link);
  free(keyboard_client);
}

ZMS_EXPORT struct wl_resource *
zms_keyboard_client_resource_create(struct zms_keyboard_client *keyboard_client,
    struct wl_client *client, uint32_t id)
{
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_keyboard_interface, 7, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  wl_list_insert(
      &keyboard_client->resource_list, wl_resource_get_link(resource));

  wl_resource_set_implementation(resource, &keyboard_interface, keyboard_client,
      zms_keyboard_client_handle_destroy);

  return resource;

err:
  return NULL;
}

ZMS_EXPORT struct wl_resource *
zms_keyboard_client_inert_resource_create(struct wl_client *client, uint32_t id)
{
  struct wl_resource *resource;

  resource = wl_resource_create(client, &wl_keyboard_interface, 7, id);
  if (resource == NULL) {
    wl_client_post_no_memory(client);
    goto err;
  }

  wl_resource_set_implementation(resource, &keyboard_interface, NULL, NULL);

  return resource;

err:
  return NULL;
}

ZMS_EXPORT struct zms_keyboard_client *
zms_keyboard_client_find(
    struct wl_client *client, struct zms_keyboard *keyboard)
{
  struct zms_keyboard_client *keyboard_client;

  wl_list_for_each(keyboard_client, &keyboard->keyboard_client_list, link)
  {
    if (keyboard_client->client == client) return keyboard_client;
  }

  return NULL;
}

ZMS_EXPORT struct zms_keyboard_client *
zms_keyboard_client_ensure(
    struct wl_client *client, struct zms_keyboard *keyboard)
{
  struct zms_keyboard_client *keyboard_client;

  keyboard_client = zms_keyboard_client_find(client, keyboard);

  if (keyboard_client)
    return keyboard_client;
  else
    return zms_keyboard_client_create(client, keyboard);
}

ZMS_EXPORT void
zms_keyboard_client_send_enter(struct zms_keyboard_client *keyboard_client,
    uint32_t serial, struct zms_view *view, struct wl_array *keys)
{
  struct wl_resource *resource;
  wl_resource_for_each(resource, &keyboard_client->resource_list)
  {
    wl_keyboard_send_enter(
        resource, serial, view->priv->surface->resource, keys);
  }
}

ZMS_EXPORT void
zms_keyboard_client_send_leave(struct zms_keyboard_client *keyboard_client,
    uint32_t serial, struct zms_view *view)
{
  struct wl_resource *resource;
  wl_resource_for_each(resource, &keyboard_client->resource_list)
  {
    wl_keyboard_send_leave(resource, serial, view->priv->surface->resource);
  }
}

ZMS_EXPORT void
zms_keyboard_client_send_key(struct zms_keyboard_client *keyboard_client,
    uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
  struct wl_resource *resource;
  wl_resource_for_each(resource, &keyboard_client->resource_list)
  {
    wl_keyboard_send_key(resource, serial, time, key, state);
  }
}

ZMS_EXPORT void
zms_keyboard_client_send_modifiers(struct zms_keyboard_client *keyboard_client,
    uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched,
    uint32_t mods_locked, uint32_t group)
{
  struct wl_resource *resource;
  wl_resource_for_each(resource, &keyboard_client->resource_list)
  {
    wl_keyboard_send_modifiers(
        resource, serial, mods_depressed, mods_latched, mods_locked, group);
  }
}
