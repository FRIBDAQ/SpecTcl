### untitled2 executable

set(untitled2test_SOURCES 
  TestRunner.cpp
  GateEditRequestTest.cpp 
  ${PROGRAM_SOURCES} )

set(untitled2test_HEADERS "")

## ------ QT BUILDING STUFF --------------------------------------------
#
# Module enabling
# uis, mocs, rccs

set(QT_USE_QTNETWORK TRUE)
#set(QT_USE_QTTEST TRUE)
#
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
#
#
# buld mocs, uis and rccs
#qt4_wrap_cpp(untitled2test_HEADERS_MOC ${untitled2test_HEADERS})
#qt4_wrap_ui(untitled2test_FORMS_HEADERS ${untitled2test_FORMS})
#qt4_add_resources(untitled2test_RESOURCES ${untitled2test_RESOURCES})

### ----- ROOT -------------------------------------------------------- 

set(ROOTCONFIGPATH $ENV{ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --cflags OUTPUT_VARIABLE _rootCFlags)
execute_process(COMMAND ${ROOTCONFIGPATH} --glibs OUTPUT_VARIABLE _rootLdFlags RESULT_VARIABLE _res)

# get rid of trailing and leading whitespace
string(STRIP ${_rootCFlags} ROOT_CXXFLAGS)
string(STRIP ${_rootLdFlags} ROOT_LDFLAGS)

# add the include paths
add_definitions(${ROOT_CXXFLAGS})


### ------- SPECTCLJSON -------------------------------------------------
# add the SpecTclJson to this

set(SPECTCLJSON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include)
set(SPECTCLJSON_LIBRARY_DIRS ${CMAKE_BINARY_DIR}/lib)

add_library(libSpecTclJson UNKNOWN IMPORTED)
set_property(TARGET libSpecTclJson
             PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libLibSpecTclJson.a)

#set(SPECTCLJSON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include)

### ------- CPPUNIT -----------------------------------------------------

pkg_search_module(untitled2test cppunit)

### ------- JSONCPP -----------------------------------------------------
#
#
#pkg_search_module(LibSpecTclJson jsoncpp)
#


########################################################################

# ADD EXECUTABLE


add_executable(untitled2test
  ${untitled2test_SOURCES} )

# we depend on some external libraries
target_link_libraries(untitled2test 
  Untitled2
  cppunit
  libSpecTclJson
  ${QT_LIBRARIES}
  ${ROOT_LDFLAGS} -lQtGSI)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/src
                    ${SPECTCLJSON_INCLUDE_DIRS})



