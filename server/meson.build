deps_zmonitors_server = [
  dep_wayland_server,
  dep_zmonitors_util,
]

srcs_zmonitors_server = [
  'compositor.c',
]

lib_zmonitors_server = static_library(
  'zmonitors-server',
  srcs_zmonitors_server,
  install: false,
  dependencies: deps_zmonitors_server,
  include_directories: public_inc,
)

dep_zmonitors_server = declare_dependency(
  link_with: lib_zmonitors_server,
  include_directories: public_inc,
)
