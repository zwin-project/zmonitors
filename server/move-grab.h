#ifndef ZMONITORS_MOVE_GRAB_H
#define ZMONITORS_MOVE_GRAB_H

#include "pointer.h"

bool zms_move_grab_start(
    struct zms_seat* seat, struct zms_view* view, uint32_t serial);

#endif  //  ZMONITORS_MOVE_GRAB_H
