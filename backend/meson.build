deps_zmonitors_backend = [
  dep_cglm,
  dep_wayland_client,
  dep_zmonitors_util,
]

srcs_zmonitors_backend = [
  'backend.c',
  'buffer.c',
  'cuboid-window.c',
  'opengl-component.c',
  'opengl-shader-program.c',
  'opengl-texture.c',
  'opengl-vertex-buffer.c',
  'virtual-object.c',
  zigen_protocol_c,
  zigen_client_protocol_h,
  zigen_shell_protocol_c,
  zigen_shell_client_protocol_h,
  zigen_opengl_protocol_c,
  zigen_opengl_client_protocol_h,
]

lib_zmonitors_backend = static_library(
  'zmonitors-backend',
  srcs_zmonitors_backend,
  install: false,
  dependencies: deps_zmonitors_backend,
  include_directories: public_inc,
)

dep_zmonitors_backend = declare_dependency(
  link_with: lib_zmonitors_backend,
  include_directories: public_inc,
)
