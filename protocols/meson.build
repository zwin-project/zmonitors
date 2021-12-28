scanner_path = dep_wayland_scanner.get_variable(pkgconfig: 'wayland_scanner')
wayland_protocols_dir = dep_wayland_protocols.get_variable(pkgconfig: 'pkgdatadir')
zgn_protocol_dir = dep_zgn_protocol.get_variable(pkgconfig: 'pkgdatadir')

generated_protocols = {
  'xdg-shell': files(wayland_protocols_dir / 'stable/xdg-shell/xdg-shell.xml'),
  'xdg-output': files(wayland_protocols_dir / 'unstable/xdg-output/xdg-output-unstable-v1.xml'),
  'zigen': files(zgn_protocol_dir / 'zigen.xml'),
  'zigen-shell': files(zgn_protocol_dir / 'zigen-shell.xml'),
  'zigen-opengl': files(zgn_protocol_dir / 'zigen-opengl.xml'),
}

foreach name, path : generated_protocols
  foreach output_type : [ 'server-header', 'client-header', 'private-code' ]
    if output_type == 'client-header'
      output_file = '@0@-client-protocol.h'.format(name)
    elif output_type == 'server-header'
      output_file = '@0@-server-protocol.h'.format(name)
    else
      output_file = '@0@-protocol.c'.format(name)
    endif

    var_name = output_file.underscorify()
    target = custom_target(
      '@0@ @1@'.format(name, output_type),
      command: [ scanner_path, output_type, '@INPUT@', '@OUTPUT@'],
      input: path,
      output: output_file,
    )

    set_variable(var_name, target)
  endforeach
endforeach
