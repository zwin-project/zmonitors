deps_zmonitors_backend = [
  dep_wayland_client,
  dep_zmonitors_util,
]

srcs_zmonitors_backend = [
  'backend.c',
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
