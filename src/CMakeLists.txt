# CMakeLists.txt for building the actual SpecTclJson library

# add the include paths
add_definitions(${ROOT_CXXFLAGS})

set(libSpecTclJson_INCLUDES HistFactory.h 
                            HistInfo.h 
                            JsonParser.h
                            HistFiller.h
                            BinInfo.h
                            ParameterInfo.h
                            GateInfo.h
                            GateCmdParser.h
                            ParameterCmdParser.h)

set(libSpecTclJson_SOURCES  HistFactory.cpp 
                            HistInfo.cpp 
                            JsonParser.cpp
                            HistFiller.cpp
                            ParameterInfo.cpp
                            GateInfo.cpp
                            GateCmdParser.cpp
                            ParameterCmdParser.cpp)
# build
add_library(LibSpecTclJson STATIC ${libSpecTclJson_INCLUDES}
                                  ${libSpecTclJson_SOURCES})

include_directories(${JSONCPP_INCLUDE_DIRS})

# we depend on some external libraries
target_link_libraries(LibSpecTclJson ${ROOT_LDFLAGS} jsoncpp)

install(TARGETS LibSpecTclJson DESTINATION lib)
install(FILES ${libSpecTclJson_INCLUDES} DESTINATION include)
