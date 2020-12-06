cmake_minimum_required(VERSION 3.17)
project(aoc2020)

set(CMAKE_CXX_STANDARD 20)

include_directories(aoc2020)

find_package(Threads)

function(day name)
    add_executable(${name} aoc2020/runner.cpp aoc2020/${name}.cpp)
endfunction()

day(day01)
day(day02)
day(day03)
day(day04)
day(day05)

add_executable(tests
        tests/test02.cpp
        tests/input_views.cpp
        )
target_link_libraries(tests
        PRIVATE gtest gtest_main Threads::Threads
        )
