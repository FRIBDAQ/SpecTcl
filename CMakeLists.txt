cmake_minimum_required(VERSION 2.8)

project(viewer)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11")

find_package(Qt4 REQUIRED)


set(CMAKE_UTILS_DIR ${CMAKE_SOURCE_DIR}/cmake_utils)
include(${CMAKE_UTILS_DIR}/FindPkgConfig.cmake)


enable_testing()

add_subdirectory(src)
add_subdirectory(test)

add_custom_target(test COMMAND untitled2test)




