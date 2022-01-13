#include "app.h"

#include <signal.h>

#include "monitor.h"

static void
zms_app_gain_ray_cap(void* data)
{
  struct zms_app* app = data;

  zms_seat_init_pointer(app->compositor->seat);
}

static void
zms_app_lose_ray_cap(void* data)
{
  struct zms_app* app = data;

  zms_seat_release_pointer(app->compositor->seat);
}

static const struct zms_backend_interface backend_interface = {
    .gain_ray_capability = zms_app_gain_ray_cap,
    .lose_ray_capability = zms_app_lose_ray_cap,
};

static int
handle_backend_event(int fd, uint32_t mask, void* data)
{
  Z_UNUSED(fd);
  struct zms_app* app = data;
  int count = 0;

  if ((mask & WL_EVENT_HANGUP) || (mask & WL_EVENT_ERROR)) {
    wl_display_terminate(app->compositor->display);
    return 0;
  }

  if (mask & WL_EVENT_READABLE) count = zms_backend_dispatch(app->backend);
  if (mask & WL_EVENT_WRITABLE) zms_backend_flush(app->backend);
  if (mask == 0) {
    count = zms_backend_dispatch_pending(app->backend);
    zms_backend_flush(app->backend);
  }

  if (count < 0) {
    wl_display_terminate(app->compositor->display);
    return 0;
  }

  return count;
}

static int
on_term_signal(int signal_number, void* data)
{
  struct zms_app* app = data;
  zms_log("\ncaught signal %d\n", signal_number);
  wl_display_terminate(app->compositor->display);
  return 0;
}

ZMS_EXPORT struct zms_app*
zms_app_create()
{
  struct zms_app* app;
  struct zms_compositor* compositor;
  struct zms_backend* backend;
  struct zms_monitor* primary_monitor;
  struct zms_screen_size screen_size = {1920, 1080};
  struct wl_event_loop* loop;
  int backend_fd;
  struct wl_event_source* backend_event_source;
  struct wl_event_source* signals[3];

  app = zalloc(sizeof *app);
  if (app == NULL) {
    zms_log("failed to allocate memroy\n");
    goto err;
  }

  compositor = zms_compositor_create();
  if (compositor == NULL) {
    zms_log("failed to create a zms_compositor\n");
    goto err_compositor;
  }

  backend = zms_backend_create(app, &backend_interface);
  if (backend == NULL) {
    zms_log("failed to create a zms_backend\n");
    goto err_backend;
  }

  app->compositor = compositor;
  app->backend = backend;

  if (zms_backend_connect(backend, "zigen-0") == false) {
    zms_log("failed to connect zigen server\n");
    goto err_connect;
  }

  primary_monitor = zms_monitor_create(backend, compositor, screen_size);
  if (primary_monitor == NULL) {
    zms_log("failed to create a primary monitor\n");
    goto err_monitor;
  }
  app->primary_monitor = primary_monitor;

  loop = wl_display_get_event_loop(compositor->display);
  backend_fd = zms_backend_get_fd(backend);
  backend_event_source = wl_event_loop_add_fd(
      loop, backend_fd, WL_EVENT_READABLE, handle_backend_event, app);

  if (backend_event_source == NULL) {
    zms_log("failed to create a event source\n");
    goto err_event_source;
  }
  app->backend_event_source = backend_event_source;

  signals[0] = wl_event_loop_add_signal(loop, SIGTERM, on_term_signal, app);
  signals[1] = wl_event_loop_add_signal(loop, SIGINT, on_term_signal, app);
  signals[2] = wl_event_loop_add_signal(loop, SIGQUIT, on_term_signal, app);

  if (!signals[0] || !signals[1] || !signals[2]) {
    zms_log("failed to create singal event sources\n");
    goto err_signal;
  }

  return app;

err_signal:
err_event_source:
  zms_monitor_destroy(primary_monitor);

err_monitor:
err_connect:
  zms_backend_destroy(backend);

err_backend:
  zms_compositor_destroy(compositor);

err_compositor:
  free(app);

err:
  return NULL;
}

ZMS_EXPORT void
zms_app_destroy(struct zms_app* app)
{
  zms_monitor_destroy(app->primary_monitor);
  zms_backend_destroy(app->backend);
  zms_compositor_destroy(app->compositor);
  free(app);
}

ZMS_EXPORT void
zms_app_run(struct zms_app* app)
{
  zms_backend_flush(app->backend);
  wl_display_run(app->compositor->display);
}
