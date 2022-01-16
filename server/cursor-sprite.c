#include "cursor-sprite.h"

#include <pixman-1/pixman.h>
#include <zmonitors-util.h>

#include "pixman-helper.h"

static void
zms_cursor_sprite_update_view_origin(struct zms_cursor_sprite *sprite)
{
  struct zms_pointer *pointer = sprite->pointer;

  if (pointer->output == NULL) return;

  zms_view_set_origin(sprite->surface->view, pointer->x - sprite->hotspot.x,
      pointer->y - sprite->hotspot.y);
}

static void
zms_cursor_sprite_surface_destroy_handler(
    struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_cursor_sprite *cursor_sprite;

  cursor_sprite =
      wl_container_of(listener, cursor_sprite, surface_destroy_listener);

  zms_cursor_sprite_destroy(cursor_sprite);
}

static void
zms_cursor_sprite_surface_commit_handler(
    struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_cursor_sprite *cursor_sprite;
  struct zms_view *view;

  cursor_sprite =
      wl_container_of(listener, cursor_sprite, surface_commit_listener);

  view = cursor_sprite->surface->view;

  if (cursor_sprite->pointer->output && zms_view_has_image(view) &&
      zms_view_is_mapped(view) == false) {
    zms_cursor_sprite_update_view_origin(cursor_sprite);
    zms_output_map_view(
        cursor_sprite->pointer->output, view, ZMS_OUTPUT_CURSOR_LAYER_INDEX);
  }
}

static void
zms_cursor_sprite_pointer_destroy_handler(
    struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  struct zms_cursor_sprite *cursor_sprite;

  cursor_sprite =
      wl_container_of(listener, cursor_sprite, pointer_destroy_listener);

  zms_cursor_sprite_destroy(cursor_sprite);
}

static void
zms_cursor_sprite_pointer_moved_handler(
    struct zms_listener *listener, void *data)
{
  Z_UNUSED(data);
  pixman_region32_t damage, old_region, new_region;
  struct zms_cursor_sprite *cursor_sprite;
  struct zms_view *view;

  cursor_sprite =
      wl_container_of(listener, cursor_sprite, pointer_moved_listener);
  view = cursor_sprite->surface->view;

  if (zms_view_is_mapped(view) == false) return;

  if (cursor_sprite->pointer->output == NULL) {
    zms_output_unmap_view(view->priv->output, view);
    return;
  }

  pixman_region32_init(&damage);
  pixman_region32_init_view_global(&old_region, cursor_sprite->surface->view);

  zms_cursor_sprite_update_view_origin(cursor_sprite);

  pixman_region32_init_view_global(&new_region, cursor_sprite->surface->view);

  pixman_region32_union(&damage, &old_region, &new_region);

  zms_output_render(view->priv->output, &damage);

  pixman_region32_fini(&damage);
  pixman_region32_fini(&old_region);
  pixman_region32_fini(&new_region);
}

ZMS_EXPORT struct zms_cursor_sprite *
zms_cursor_sprite_create(struct zms_pointer *pointer,
    struct zms_surface *surface, int32_t hotspot_x, int32_t hotspot_y)
{
  struct zms_cursor_sprite *cursor_sprite;

  cursor_sprite = zalloc(sizeof *cursor_sprite);
  if (cursor_sprite == NULL) goto err;

  if (zms_surface_set_role(surface, SURFACE_ROLE_CURSOR, surface->resource,
          WL_POINTER_ERROR_ROLE) < 0) {
    goto err_role;
  }
  surface->role_object = cursor_sprite;

  cursor_sprite->pointer = pointer;
  cursor_sprite->surface = surface;

  zms_signal_init(&cursor_sprite->destroy_signal);

  cursor_sprite->surface_destroy_listener.notify =
      zms_cursor_sprite_surface_destroy_handler;
  zms_signal_add(
      &surface->destroy_signal, &cursor_sprite->surface_destroy_listener);

  cursor_sprite->surface_commit_listener.notify =
      zms_cursor_sprite_surface_commit_handler;
  zms_signal_add(
      &surface->commit_signal, &cursor_sprite->surface_commit_listener);

  cursor_sprite->pointer_destroy_listener.notify =
      zms_cursor_sprite_pointer_destroy_handler;
  zms_signal_add(
      &pointer->destroy_signal, &cursor_sprite->pointer_destroy_listener);

  cursor_sprite->pointer_moved_listener.notify =
      zms_cursor_sprite_pointer_moved_handler;
  zms_signal_add(
      &pointer->moved_signal, &cursor_sprite->pointer_moved_listener);

  cursor_sprite->hotspot.x = hotspot_x;
  cursor_sprite->hotspot.y = hotspot_y;

  if (pointer->output && zms_view_has_image(cursor_sprite->surface->view)) {
    zms_cursor_sprite_update_view_origin(cursor_sprite);
    zms_output_map_view(pointer->output, cursor_sprite->surface->view,
        ZMS_OUTPUT_CURSOR_LAYER_INDEX);
  }

  return cursor_sprite;

err_role:
  free(cursor_sprite);

err:
  return NULL;
}

ZMS_EXPORT void
zms_cursor_sprite_destroy(struct zms_cursor_sprite *cursor_sprite)
{
  zms_output_unmap_view(
      cursor_sprite->surface->view->priv->output, cursor_sprite->surface->view);
  zms_signal_emit(&cursor_sprite->destroy_signal, NULL);
  wl_list_remove(&cursor_sprite->surface_destroy_listener.link);
  wl_list_remove(&cursor_sprite->surface_commit_listener.link);
  wl_list_remove(&cursor_sprite->pointer_destroy_listener.link);
  wl_list_remove(&cursor_sprite->pointer_moved_listener.link);
  free(cursor_sprite);
}

ZMS_EXPORT void
zms_cursor_sprite_update_hotspot(
    struct zms_cursor_sprite *sprite, int32_t hotspot_x, int32_t hotspot_y)
{
  pixman_region32_t damage, old_region, new_region;
  sprite->hotspot.x = hotspot_x;
  sprite->hotspot.y = hotspot_y;

  if (zms_view_is_mapped(sprite->surface->view) == false) return;

  pixman_region32_init(&damage);
  pixman_region32_init_view_global(&old_region, sprite->surface->view);

  zms_cursor_sprite_update_view_origin(sprite);

  pixman_region32_init_view_global(&new_region, sprite->surface->view);

  pixman_region32_union(&damage, &old_region, &new_region);

  zms_output_render(sprite->surface->view->priv->output, &damage);

  pixman_region32_fini(&damage);
  pixman_region32_fini(&old_region);
  pixman_region32_fini(&new_region);
}
