subdir('Common')
subdir('Memory')
subdir('Network')
subdir('Http')

libserver = static_library(
  'server',
  link_with: [ libnetwork, libhttp ]
  )

subdir('test')

argparse_dep = dependency('argparse', fallback: ['argparse', 'argparse_dep'])
boost_dep = dependency('boost', modules: ['thread'])

e = executable(
  'server',
  'main.cpp',
  dependencies: [ argparse_dep, boost_dep ],
  link_with: [ libserver ]
)
