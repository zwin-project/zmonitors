#ifndef ZMONITORS_UI_H
#define ZMONITORS_UI_H

#include <zmonitors-backend.h>

struct zms_ui_paper;

struct zms_ui_paper* zms_ui_paper_create(
    struct zms_cuboid_window* cuboid_window);

void zms_ui_paper_destroy(struct zms_ui_paper* paper);

#endif  //  ZMONITORS_UI_H
