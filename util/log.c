#include <stdarg.h>
#include <stdio.h>
#include <zmonitors-util.h>

ZMS_EXPORT int
zms_log(const char *fmt, ...)
{
  int l;
  va_list argp;

  va_start(argp, fmt);
  l = vfprintf(stderr, fmt, argp);
  va_end(argp);

  return l;
}
