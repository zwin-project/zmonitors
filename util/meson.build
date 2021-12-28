deps_zmonitors_util = []

srcs_zmonitors_util = [
  'log.c',
]

lib_zmonitors_util = static_library(
  'zmonitors-util',
  srcs_zmonitors_util,
  install: false,
  dependencies: deps_zmonitors_util,
  include_directories: public_inc,
)

dep_zmonitors_util = declare_dependency(
  link_with: lib_zmonitors_util,
  include_directories: public_inc,
)
