pkg_search_module(unittests cppunit)

# include ROOT stuff
set(ROOTCONFIGPATH $ENV{ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --cflags OUTPUT_VARIABLE _rootCFlags)
execute_process(COMMAND ${ROOTCONFIGPATH} --libs OUTPUT_VARIABLE _rootLdFlags RESULT_VARIABLE _res)

# get rid of trailing and leading whitespace
string(STRIP ${_rootCFlags} ROOT_CXXFLAGS)
string(STRIP ${_rootLdFlags} ROOT_LDFLAGS)

# add the include paths
add_definitions(${ROOT_CXXFLAGS})

include_directories(${CMAKE_SOURCE_DIR}/src ${PROJECT_BINARY_DIR})

add_executable(unittests
                TestRunner.cpp 
                HistFactoryTest.cpp 
                HistInfoTest.cpp 
                JsonParserTest.cpp
                HistFillerTest.cpp
                GateInfoTest.cpp
                GateCmdParserTest.cpp
                ParameterCmdParserTest.cpp
                json/content1d.json
                json/gate1d.json
                json/list1d_detail.json
                json/list1d.json
                json/list2d.json
                json/paramList.json
                json/test2.json
                json/test3.json
                json/gate2dc.json
                json/gate2db.json)

target_link_libraries(unittests cppunit ${ROOT_LDFLAGS} jsoncpp LibSpecTclJson)


#file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/json)
#file(GLOB JSON_TESTS ./json/*json)
#file(COPY ${JSON_TESTS} DESTINATION ${PROJECT_BINARY_DIR}/json/)

install(TARGETS unittests DESTINATION bin)

