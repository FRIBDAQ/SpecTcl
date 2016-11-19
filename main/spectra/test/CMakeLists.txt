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
  HistogramBundleTest.cpp )

set(spectratest_HEADERS "")

## ------ QT BUILDING STUFF --------------------------------------------
#
# Module enabling
# uis, mocs, rccs

set(QT_USE_QTNETWORK TRUE)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

### ------- CPPUNIT -----------------------------------------------------

pkg_search_module(spectratest cppunit)

########################################################################

# ADD EXECUTABLE

add_executable(spectratest
  ${spectratest_SOURCES} )

# we depend on some external libraries
target_link_libraries(spectratest 
  Spectra
  cppunit
  LibSpecTclJson
  ${QT_LIBRARIES}
  ${ROOT_LDFLAGS} -lQtGSI ROOT)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${SPECTCLJSON_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIRS})

add_definitions(${ROOT_CXXFLAGS})


message(STATUS ${ROOT_CXXFLAGS})
message(STATUS ${ROOT_LDFLAGS})

