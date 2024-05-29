if tests.disabled()
    subdir_done()
endif

test_sources = [
    'Parser.cpp'
]

e = executable('tests', test_sources,
                dependencies: [gtest_main_dep],
                include_directories: [inc],
                link_with: [libhttp])

test('Http', e)