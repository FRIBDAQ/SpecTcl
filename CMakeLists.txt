cmake_minimum_required(VERSION 2.6)

project(SpecTclJson)

## Enable C++ 11 support
set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11")

## Test for C++11 Support
# Taken from http://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake
# response by Matthias Valentin.
#
# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()
############## end of stackoverflow solution

# create config file
configure_file("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/config.h")

#--------------------------------------------------------
# include ROOT stuff
set(ROOTCONFIGPATH $ENV{ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --cflags OUTPUT_VARIABLE _rootCFlags)
execute_process(COMMAND ${ROOTCONFIGPATH} --libs OUTPUT_VARIABLE _rootLdFlags RESULT_VARIABLE _res)

# get rid of trailing and leading whitespace
string(STRIP ${_rootCFlags} ROOT_CXXFLAGS)
string(STRIP ${_rootLdFlags} ROOT_LDFLAGS)

# --------------------------------------------------------
# pkg-config search
include(${CMAKE_SOURCE_DIR}/build/FindPkgConfig.cmake)

pkg_search_module(JSONCPP jsoncpp)
if(${JSONCPP_FOUND})
  message("Found jsoncpp in pkg-config")
  set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIRS})
else(${JSONCPP_FOUND})
  message("Failed to find jsoncpp in pkg-config... retrieving and building")
  include(ExternalProject)
  set(USE_PROJECT_CMAKE_MODULE_PATH "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
  ExternalProject_Add(JSONCPP 
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG 0.10.1
    UPDATE_COMMAND ""
    TEST_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      ${USE_PROJECT_CMAKE_MODULE_PATH}
  )

  set(JSONCPP_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include)
  
  add_library(jsoncpp UNKNOWN IMPORTED)
  set_property(TARGET jsoncpp
             PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libjsoncpp.a)
endif(${JSONCPP_FOUND})


add_subdirectory(src)
add_subdirectory(test)


add_custom_target(test COMMAND unittests)
