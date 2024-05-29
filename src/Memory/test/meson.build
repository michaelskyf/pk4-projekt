if tests.disabled()
    subdir_done()
endif

test_sources = [
    'Mempool.cpp'
]

e = executable('tests', test_sources,
                dependencies: [gtest_main_dep],
                include_directories: [inc])

test('Memory', e)