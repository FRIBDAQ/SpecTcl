# CMakeLists.txt for building the actual SpecTclJson library

# include ROOT stuff
set(ROOTCONFIGPATH $ENV{ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --cflags OUTPUT_VARIABLE _rootCFlags)
execute_process(COMMAND ${ROOTCONFIGPATH} --libs OUTPUT_VARIABLE _rootLdFlags RESULT_VARIABLE _res)

# get rid of trailing and leading whitespace
string(STRIP ${_rootCFlags} ROOT_CXXFLAGS)
string(STRIP ${_rootLdFlags} ROOT_LDFLAGS)

# add the include paths
add_definitions(${ROOT_CXXFLAGS})

pkg_search_module(LibSpecTclJson jsoncpp)

set(libSpecTclJson_INCLUDES HistFactory.h 
                            HistInfo.h 
                            JsonParser.h
                            HistFiller.h
                            BinInfo.h )

set(libSpecTclJson_SOURCES  HistFactory.cpp 
                            HistInfo.cpp 
                            JsonParser.cpp
                            HistFiller.cpp)
# build
add_library(LibSpecTclJson STATIC ${libSpecTclJson_INCLUDES}
                                  ${libSpecTclJson_SOURCES})


# we depend on some external libraries
target_link_libraries(LibSpecTclJson ${ROOT_LDFLAGS} jsoncpp)

install(TARGETS LibSpecTclJson DESTINATION lib)
install(FILES ${libSpecTclJson_INCLUDES} DESTINATION include)
