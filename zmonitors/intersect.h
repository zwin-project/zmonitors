#ifndef ZMONITORS_INTERSECT_H
#define ZMONITORS_INTERSECT_H

#include <cglm/cglm.h>

bool zms_interesect_ray_rect(vec3 origin, vec3 direction, vec3 v0, vec3 vx,
    vec3 vy, vec2 pos, float *distance);

#endif  //  ZMONITORS_INTERSECT_H
