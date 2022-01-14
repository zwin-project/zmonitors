#include "intersect.h"

#include <zmonitors-util.h>

/* Möller–Trumbore ray-triangle intersection algorithm */
ZMS_EXPORT bool
zms_interesect_ray_rect(vec3 origin, vec3 direction, vec3 v0, vec3 vx, vec3 vy,
    vec2 pos, float *distance)
{
  vec3 edge_x, edge_y, p, t, q;
  float det, inv_det, u, v, dist;
  const float epsilon = 0.000001f;

  glm_vec3_sub(vx, v0, edge_x);
  glm_vec3_sub(vy, v0, edge_y);

  glm_vec3_cross(direction, edge_y, p);
  det = glm_vec3_dot(edge_x, p);
  if (-epsilon < det && det < epsilon) return false;

  inv_det = 1.0f / det;

  glm_vec3_sub(origin, v0, t);

  u = inv_det * glm_vec3_dot(t, p);
  if (u < 0.0f || u > 1.0f) return false;

  glm_vec3_cross(t, edge_x, q);

  v = inv_det * glm_vec3_dot(direction, q);
  if (v < 0.0f || v > 1.0f) return false;

  dist = inv_det * glm_vec3_dot(edge_y, q);
  if (dist < epsilon) return false;

  if (distance) *distance = dist;

  pos[0] = u;
  pos[1] = v;

  return true;
}
