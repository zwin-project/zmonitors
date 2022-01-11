#include "pointer.h"

#include <zmonitors-util.h>

ZMS_EXPORT struct zms_pointer*
zms_pointer_create()
{
  struct zms_pointer* pointer;

  pointer = zalloc(sizeof *pointer);
  if (pointer == NULL) goto err;

  wl_list_init(&pointer->resource_list);

  return pointer;

err:
  return NULL;
}

ZMS_EXPORT void
zms_pointer_destroy(struct zms_pointer* pointer)
{
  // TODO: deinit element of pointer->resource_list

  free(pointer);
}
