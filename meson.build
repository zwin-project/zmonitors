project(
  'zmonitors', 'c',
  version: '0.0.1',
  license: 'Apache-2.0',
  meson_version: '>= 0.52.1',
  default_options:  [ 'warning_level=3', 'werror=true', 'optimization=2', 'c_std=gnu11' ],
)

if get_option('debug')
  add_global_arguments('-DDEBUG', language: [ 'c' ])
endif
add_global_arguments('-D_GNU_SOURCE', language: [ 'c' ])

public_inc = include_directories('include')

# dependencies
dep_wayland_server = dependency('wayland-server')
dep_wayland_client = dependency('wayland-client')
dep_wayland_protocols = dependency('wayland-protocols')
dep_wayland_scanner = dependency('wayland-scanner', version: '>= 1.14.91')
dep_zgn_protocol = dependency('zigen-protocols')
dep_cglm = dependency('cglm')
dep_m = meson.get_compiler('c').find_library('m')

prog_python = import('python').find_installation('python3')
files_textify_py = files('tools/textify.py')
cmd_textify = [ prog_python, files_textify_py, '@INPUT@', '@OUTPUT@']

subdir('protocols')
subdir('util')
subdir('server')
subdir('backend')
subdir('zmonitors')
