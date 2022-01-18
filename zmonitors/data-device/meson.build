deps_zms_data_device = [
  dep_wayland_server,
  dep_zmonitors_server,
  dep_zmonitors_backend,
]

srcs_zms_data_device = [
  'manager.c',
  'data-device.c',
  'data-source.c',
  zigen_client_protocol_h,
]

lib_zms_data_device = static_library(
  'zmonitors-data-device',
  srcs_zms_data_device,
  install: false,
  dependencies: deps_zms_data_device,
  include_directories: [
    zmonitors_private_inc,
    public_inc,
  ]
)


dep_zms_data_device = declare_dependency(
  link_with: lib_zms_data_device,
)
