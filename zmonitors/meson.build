zmonitors_private_inc = include_directories('.')

subdir('ui')
subdir('monitor')

deps_zmonitors = [
  dep_zmonitors_backend,
  dep_zmonitors_server,
  dep_zmonitors_util,
  dep_zms_monitor,
]

srcs_zmonitors = [
  'app.c',
  'main.c',
]

executable(
  'zmonitors',
  srcs_zmonitors,
  install: true,
  pie: true,
  dependencies: deps_zmonitors,
)
