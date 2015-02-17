cmake_minimum_required(VERSION 2.6)

project(SpecTclJson)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11")

# pkg-config search
include(${CMAKE_SOURCE_DIR}/build/FindPkgConfig.cmake)

add_subdirectory(src)
add_subdirectory(test)
