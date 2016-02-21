cmake_minimum_required(VERSION 2.8)

project(Spectra)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11")

find_package(Qt4 REQUIRED)


set(CMAKE_UTILS_DIR ${CMAKE_SOURCE_DIR}/cmake_utils)
include(${CMAKE_UTILS_DIR}/FindPkgConfig.cmake)

# The following is an excerpt from CMAKE docs (https://cmake.org/Wiki/CMake_RPATH_handling)
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

#### ----- ROOT -------------------------------------------------------- 

if(NOT DEFINED ENV{ROOTSYS} ) 
  set(ROOTSYSENV $ENV{ROOTSYS})
endif()

if((NOT DEFINED ROOTSYS)) 
  if (NOT DEFINED ROOTSYSENV)
    message(FATAL_ERROR "ROOTSYS variable is required.")
  else(DEFINED ROOTSYSENV)
    set(ROOTSYS $ROOTSYSENV)
  endif()
endif()

set(ROOTCONFIGPATH ${ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --auxcflags OUTPUT_VARIABLE ROOT_CXXFLAGS)
execute_process(COMMAND ${ROOTCONFIGPATH} --incdir OUTPUT_VARIABLE ROOT_INCLUDE_DIRS)
execute_process(COMMAND ${ROOTCONFIGPATH} --glibs OUTPUT_VARIABLE ROOT_LDFLAGS RESULT_VARIABLE _res)
execute_process(COMMAND ${ROOTCONFIGPATH} --libdir OUTPUT_VARIABLE ROOT_LIBRARY_DIRS)

# get rid of trailing and leading whitespace
string(STRIP ${ROOT_CXXFLAGS} ROOT_CXXFLAGS)
string(STRIP ${ROOT_INCLUDE_DIRS} ROOT_INCLUDE_DIRS)
string(STRIP ${ROOT_LDFLAGS} ROOT_LDFLAGS)
string(STRIP ${ROOT_LIBRARY_DIRS} ROOT_LIBRARY_DIRS)
add_library(ROOT SHARED IMPORTED)
set_target_properties(ROOT PROPERTIES IMPORTED_LOCATION ${ROOT_LIBRARY_DIRS}/libCore.so)

message(STATUS ${ROOT_CXXFLAGS})
message(STATUS ${ROOT_LDFLAGS})

enable_testing()

add_subdirectory(spectcljson)
add_subdirectory(src)
add_subdirectory(test)

add_custom_target(check COMMAND spectratest)




