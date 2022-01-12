deps_zms_monitor = [
  dep_cglm,
  dep_zmonitors_server,
  dep_zms_ui,
]

screen_vertex_glsl = custom_target(
  'screen-vert.h',
  command: cmd_textify + [ '-n', 'screen_vertex_shader' ],
  input: 'screen.vert',
  output: 'screen-vert.h'
)

screen_fragment_glsl = custom_target(
  'screen-frag.h',
  command: cmd_textify + [ '-n', 'screen_fragment_shader' ],
  input: 'screen.frag',
  output: 'screen-frag.h'
)

control_bar_vertex_glsl = custom_target(
  'control-bar-vert.h',
  command: cmd_textify + [ '-n', 'control_bar_vertex_shader' ],
  input: 'control-bar.vert',
  output: 'control-bar-vert.h'
)

control_bar_fragment_glsl = custom_target(
  'control-bar-frag.h',
  command: cmd_textify + [ '-n', 'control_bar_fragment_shader' ],
  input: 'control-bar.frag',
  output: 'control-bar-frag.h'
)

srcs_zms_monitor = [
  'control-bar.c',
  'monitor.c',
  'screen.c',
  control_bar_fragment_glsl,
  control_bar_vertex_glsl,
  screen_fragment_glsl,
  screen_vertex_glsl,
  zigen_opengl_client_protocol_h,
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
