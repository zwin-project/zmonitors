#ifndef ZMONITORS_TYPES_H
#define ZMONITORS_TYPES_H

#include <stdint.h>

struct zms_screen_size {
  int32_t width, height;
};

struct zms_bgra {
  uint8_t b, g, r, a;
};

#endif  //  ZMONITORS_SERVER_TYPES_H
