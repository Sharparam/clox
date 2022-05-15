project('clox', 'c',
  version : '0.1.0',
  default_options : [
    'c_std=c11',
    'warning_level=3'
  ])

inc = include_directories('include')
src = [
  'src/main.c',
  'src/io.c',
  'src/options.c',
  'src/chunk.c',
  'src/memory.c',
  'src/debug.c',
  'src/value.c',
  'src/scanner.c',
  'src/compiler.c',
  'src/vm.c'
]

version_parts = meson.project_version().split('.')

conf_data = configuration_data({
  'CLOX_VERSION_MAJOR': version_parts[0],
  'CLOX_VERSION_MINOR': version_parts[1],
  'CLOX_VERSION_PATCH': version_parts[2],
  'CLOX_DEBUG_PRINT_CODE': get_option('print_code'),
  'CLOX_DEBUG_TRACE_EXECUTION': get_option('trace')
})
conf_data.set_quoted('CLOX_VERSION_STR', meson.project_version())
configure_file(output: 'config.h', configuration: conf_data)

exe = executable('clox', src,
  include_directories : inc,
  install : true)

test('basic', exe)
