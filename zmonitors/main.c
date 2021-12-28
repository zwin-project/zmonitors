#include "app.h"

int
main()
{
  struct zms_app *app;
  int exit_code = EXIT_FAILURE;

  app = zms_app_create();
  if (app == NULL) goto out;

  zms_app_run(app);

  exit_code = EXIT_SUCCESS;

  /* exit */

  zms_app_destroy(app);

out:
  if (exit_code == EXIT_SUCCESS) zms_log("zmonitors exited successfully.\n");
  return exit_code;
}
