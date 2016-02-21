pkg_search_module(unittests cppunit)

# add the include paths
add_definitions(${ROOT_CXXFLAGS})

include_directories(${CMAKE_SOURCE_DIR}/src ${PROJECT_BINARY_DIR} ${JSONCPP_INCLUDE_DIRS})

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
                json/content2d.json
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

install(TARGETS unittests DESTINATION bin)

