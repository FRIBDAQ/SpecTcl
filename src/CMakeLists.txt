cmake_minimum_required(VERSION 2.6)


project(SpecTclJson)

# include ROOT stuff
set(ROOTCONFIGPATH $ENV{ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --cflags OUTPUT_VARIABLE _rootCFlags)
execute_process(COMMAND ${ROOTCONFIGPATH} --libs OUTPUT_VARIABLE _rootLdFlags RESULT_VARIABLE _res)

# get rid of trailing and leading whitespace
string(STRIP ${_rootCFlags} ROOT_CXXFLAGS)
string(STRIP ${_rootLdFlags} ROOT_LDFLAGS)

# add the include paths
add_definitions(${ROOT_CXXFLAGS})

pkg_search_module(Test jsoncpp)

# build
add_library(SpecTclJson HistFactory.cpp HistFactory.h)

# we depend on some external libraries
target_link_libraries(SpecTclJson ${ROOT_LDFLAGS})
target_link_libraries(SpecTclJson jsoncpp)
