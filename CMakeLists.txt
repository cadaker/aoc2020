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
day(day06)
day(day07)
day(day08)
day(day09)
day(day10)
day(day11)
day(day12)
day(day13)
day(day14)
day(day15)
day(day16)
day(day17)
day(day18)
day(day19)
day(day20)
day(day21)
day(day22)
day(day23)
day(day24)
day(day25)

add_executable(tests
        tests/test02.cpp
        tests/input_helpers.cpp
        tests/grid.cpp
        aoc2020/day02.cpp
        )
target_link_libraries(tests
        PRIVATE gtest gtest_main Threads::Threads
        )
