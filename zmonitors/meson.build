zmonitors_private_inc = include_directories('.')

subdir('ui')
subdir('monitor')

deps_zmonitors = [
  dep_wayland_server,
  dep_zmonitors_backend,
  dep_zmonitors_server,
  dep_zmonitors_util,
  dep_zms_monitor,
]

srcs_zmonitors = [
  'app.c',
  'data-offer-proxy.c',
  'data-source-proxy.c',
  'intersect.c',
  'main.c',
  zigen_client_protocol_h,
]

executable(
  'zmonitors',
  srcs_zmonitors,
  install: true,
  pie: true,
  dependencies: deps_zmonitors,
)
