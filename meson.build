project('http-srver', 'cpp',
    version : '1.0',
    license : 'gpl',
    meson_version : '>= 0.61.2',
    default_options : [ 'warning_level=everything', 'buildtype=debug', 'cpp_std=c++23', 'werror=false' ]
)

cpp_flags = [

]

cpp_warn_blacklist = [
  '-Wno-suggest-attribute=pure', '-Wno-padded', '-Wno-c++98-compat', '-Wno-c++98-compat-pedantic',
  '-Wno-global-constructors', '-Wno-newline-eof', '-Wno-null-dereference', '-Wno-switch-default',
  '-Wno-missing-declarations', '-Wno-unused-parameter', '-Wno-effc++', '-Wno-inline', '-Wno-cast-align',
  '-Wno-abi-tag', '-Wno-unused-const-variable'
]

compiler = meson.get_compiler('cpp')
cpp_all_flags = cpp_flags + cpp_warn_blacklist

foreach f :cpp_all_flags
  if compiler.has_argument(f)
    add_project_arguments(f, language : 'cpp')
  endif
endforeach

inc = include_directories('src')

tests = get_option('tests')
if tests.enabled()
    gtest_main_dep = dependency('gtest', main : true, fallback: ['gtest', 'gtest_main_dep'], required: true)
endif

subdir('src')
# subdir('docs')
