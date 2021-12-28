zmonitors_private_inc = include_directories('.')

subdir('monitor')

deps_zmonitors = [
  dep_zmonitors_backend,
  dep_zmonitors_server,
  dep_zmonitors_util,
  dep_zms_monitor,
]

src_zmonitors = [
  'app.c',
  'main.c',
]

executable(
  'zmonitors',
  src_zmonitors,
  install: true,
  pie: true,
  dependencies: deps_zmonitors,
)
