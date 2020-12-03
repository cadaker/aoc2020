cmake_minimum_required(VERSION 3.17)
project(aoc2020)

set(CMAKE_CXX_STANDARD 20)

include_directories(aoc2020)

find_package(Threads)

add_executable(day01
        aoc2020/day01.cpp
        )

add_executable(day02
        aoc2020/day02.cpp
        )

add_executable(day03
        aoc2020/day03.cpp
        )

add_executable(tests
        tests/test02.cpp)
target_link_libraries(tests
        PRIVATE gtest gtest_main Threads::Threads)
