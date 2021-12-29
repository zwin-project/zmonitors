#ifndef ZMONITORS_BACKEND_CUBOID_WINDOW_H
#define ZMONITORS_BACKEND_CUBOID_WINDOW_H

#include <zigen-shell-client-protocol.h>

#include "virtual-object.h"

struct zms_cuboid_window_private {
  struct zgn_cuboid_window *proxy;
  void *user_data;
};

#endif  //  ZMONITORS_BACKEND_CUBOID_WINDOW_H
