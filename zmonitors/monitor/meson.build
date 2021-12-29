deps_zms_monitor = [
  dep_cglm,
  dep_zmonitors_server,
  dep_zms_ui,
]

srcs_zms_monitor = [
  'monitor.c',
]

lib_zms_monitor = static_library(
  'zmonitors-zms-monitor',
  srcs_zms_monitor,
  install: false,
  dependencies: deps_zms_monitor,
  include_directories: [
    zmonitors_private_inc,
    public_inc,
  ],
)

dep_zms_monitor = declare_dependency(
  link_with: lib_zms_monitor,
)
