#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <zmonitors.h>

volatile sig_atomic_t run = false;

void
sig_app_end_handler(int signum)
{
  Z_UNUSED(signum);
  run = false;
}

bool
register_signal_handlers()
{
  if (signal(SIGINT, sig_app_end_handler) == SIG_ERR) return false;
  if (signal(SIGTERM, sig_app_end_handler) == SIG_ERR) return false;
  if (signal(SIGQUIT, sig_app_end_handler) == SIG_ERR) return false;
  return true;
}

int
main()
{
  int exit_code = EXIT_FAILURE;
  if (!register_signal_handlers()) goto out;

  struct zms_compositor *compositor;

  compositor = zms_compositor_create();
  if (compositor == NULL) goto out;

  run = true;
  while (run) {
    zms_compositor_flush_clients(compositor);
    zms_compositor_dispatch_event(compositor, 0);
  }

  exit_code = EXIT_SUCCESS;

  /* exit */

  zms_compositor_destroy(compositor);

out:
  if (exit_code == EXIT_SUCCESS) zms_log("\nzmonitors exited successfully.\n");
  return exit_code;
}
