cmake_minimum_required(VERSION 2.6)

project(SpecTclJson)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11")

configure_file("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/config.h")

# pkg-config search
include(${CMAKE_SOURCE_DIR}/build/FindPkgConfig.cmake)

add_subdirectory(src)
add_subdirectory(test)


add_custom_target(test COMMAND unittests)
