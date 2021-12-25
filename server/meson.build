deps_zmonitors_server = [
  dep_wayland_server,
  dep_zmonitors_util,
]

srcs_zmonitors_server = [
  'compositor.c',
  'output.c',
  'seat.c',
  'surface.c',
  'view.c',
  'xdg-surface.c',
  'xdg-toplevel.c',
  'xdg-wm-base.c',
  xdg_shell_protocol_c,
  xdg_shell_server_protocol_h,
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
