cmake_minimum_required(VERSION 3.14)
project(kmds C)

set(CMAKE_C_STANDARD 99)

add_executable(
        kmds_tests
        src/tests/tests.c
        src/tests/test_array.c
        src/tests/test_list.c
        src/tests/test_map.c
)
target_include_directories(kmds_tests PUBLIC include)

add_executable(kmds_example src/examples/example.c)
target_include_directories(kmds_example PUBLIC include)
