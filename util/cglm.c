#include <cglm/cglm.h>
#include <string.h>
#include <wayland-util.h>
#include <zmonitors-util.h>

static const size_t vec3_size = sizeof(vec3);
static const size_t mat4_size = sizeof(mat4);
static const size_t versor_size = sizeof(versor);

ZMS_EXPORT int
glm_vec3_from_wl_array(vec3 v, struct wl_array* array)
{
  float* data = array->data;
  if (array->size != vec3_size) return -1;
  memcpy(v, data, vec3_size);
  return 0;
}

ZMS_EXPORT void
glm_vec3_to_wl_array(vec3 v, struct wl_array* array)
{
  if (array->alloc > 0) {
    wl_array_release(array);
    wl_array_init(array);
  }
  float* data = wl_array_add(array, vec3_size);
  memcpy(data, v, vec3_size);
}

ZMS_EXPORT int
glm_versor_from_wl_array(versor v, struct wl_array* array)
{
  float* data = array->data;
  if (array->size != versor_size) return -1;
  memcpy(v, data, versor_size);
  glm_quat_normalize(v);
  return 0;
}

ZMS_EXPORT void
glm_versor_to_wl_array(versor v, struct wl_array* array)
{
  if (array->alloc > 0) {
    wl_array_release(array);
    wl_array_init(array);
  }
  float* data = wl_array_add(array, versor_size);
  memcpy(data, v, versor_size);
}

ZMS_EXPORT int
glm_mat4_from_wl_array(mat4 m, struct wl_array* array)
{
  float* data = array->data;
  if (array->size != mat4_size) return -1;
  memcpy(m, data, mat4_size);
  return 0;
}

ZMS_EXPORT void
glm_mat4_to_wl_array(mat4 m, struct wl_array* array)
{
  if (array->alloc > 0) {
    wl_array_release(array);
    wl_array_init(array);
  }
  float* data = wl_array_add(array, mat4_size);
  memcpy(data, m, mat4_size);
}
