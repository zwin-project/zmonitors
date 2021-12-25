dep_zmonitors = [
  dep_zmonitors_server,
  dep_zmonitors_util,
]

src_zmonitors = [
  'main.c',
]

executable(
  'zmonitors',
  src_zmonitors,
  install: true,
  pie: true,
  dependencies: dep_zmonitors,
)
