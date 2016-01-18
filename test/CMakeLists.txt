### spectra executable

set(spectratest_SOURCES 
  TestRunner.cpp
  GateEditRequestTest.cpp 
  GateDeleteRequestTest.cpp 
  GSliceTest.cpp
  GGateTest.cpp
  HistogramListTest.cpp
  GateListTest.cpp
  DockableGateManagerTest.cpp
  QTLineTest.cpp
  HistogramBundleTest.cpp
  ${PROGRAM_SOURCES} )

set(spectratest_HEADERS "")

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
#qt4_wrap_cpp(spectratest_HEADERS_MOC ${spectratest_HEADERS})
#qt4_wrap_ui(spectratest_FORMS_HEADERS ${spectratest_FORMS})
#qt4_add_resources(spectratest_RESOURCES ${spectratest_RESOURCES})

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

if(DEFINED SpecTclJsonPrefix)
    set(SPECTCLJSON_INCLUDE_DIRS ${SpecTclJsonPrefix}/include)
    set(SPECTCLJSON_LIBRARY_DIRS ${SpecTclJsonPrefix}/lib)
endif()

add_library(libSpecTclJson UNKNOWN IMPORTED)
set_property(TARGET libSpecTclJson
	     PROPERTY IMPORTED_LOCATION ${SPECTCLJSON_LIBRARY_DIRS}/libLibSpecTclJson.a)


### ------- CPPUNIT -----------------------------------------------------

pkg_search_module(spectratest cppunit)

### ------- JSONCPP -----------------------------------------------------
#
#
#pkg_search_module(LibSpecTclJson jsoncpp)
#


########################################################################

# ADD EXECUTABLE


add_executable(spectratest
  ${spectratest_SOURCES} )

# we depend on some external libraries
target_link_libraries(spectratest 
  Spectra
  cppunit
  libSpecTclJson
  ${QT_LIBRARIES}
  ${ROOT_LDFLAGS} -lQtGSI)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}/src
                    ${SPECTCLJSON_INCLUDE_DIRS})



