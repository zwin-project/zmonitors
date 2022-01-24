deps_zmonitors_server = [
  dep_m,
  dep_pixman,
  dep_wayland_server,
  dep_zmonitors_util,
]

srcs_zmonitors_server = [
  'buffer.c',
  'compositor.c',
  'cursor-sprite.c',
  'data-device.c',
  'data-device-manager.c',
  'data-offer.c',
  'data-source.c',
  'drag-grab.c',
  'frame-callback.c',
  'move-grab.c',
  'pointer.c',
  'pointer-client.c',
  'output.c',
  'pixel-buffer.c',
  'region.c',
  'seat.c',
  'surface.c',
  'view.c',
  'view-layer.c',
  'xdg-popup.c',
  'xdg-positioner.c',
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
