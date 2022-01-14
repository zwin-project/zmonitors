deps_zmonitors_util = [
  dep_cglm,
]

srcs_zmonitors_util = [
  'cglm.c',
  'fd.c',
  'log.c',
  'weak-ref.c',
]

user_deps_zmonitors_util = [
  dep_cglm,
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
  dependencies: user_deps_zmonitors_util,
)
