cmake_minimum_required(VERSION 3.12)

project(
    GB++
    VERSION 1.0
    DESCRIPTION "Nintendo GameBoy Emulator in C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++20")

INCLUDE(FindPkgConfig)

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
