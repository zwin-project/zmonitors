#ifndef ZMONITORS_BACKEND_H
#define ZMONITORS_BACKEND_H

#include <stdbool.h>
#include <zmonitors-util.h>

struct zms_backend;

struct zms_backend* zms_backend_create();

void zms_backend_destroy(struct zms_backend* backend);

bool zms_backend_connect(struct zms_backend* backend, const char* socket);

int zms_backend_get_fd(struct zms_backend* backend);

int zms_backend_dispatch(struct zms_backend* backend);

int zms_backend_flush(struct zms_backend* backend);

int zms_backend_dispatch_pending(struct zms_backend* backend);

#endif  //  ZMONITORS_BACKEND_H
