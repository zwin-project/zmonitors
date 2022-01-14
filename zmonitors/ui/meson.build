deps_zms_ui = [
  dep_wayland_server,
  dep_zmonitors_backend,
]

srcs_zms_ui = [
  'base.c',
  'root.c',
]

lib_zms_ui = static_library(
  'zmonitors-zms-ui',
  srcs_zms_ui,
  install: false,
  dependencies: deps_zms_ui,
  include_directories: [
    zmonitors_private_inc,
    public_inc,
  ],
)

dep_zms_ui = declare_dependency(
  link_with: lib_zms_ui,
)
