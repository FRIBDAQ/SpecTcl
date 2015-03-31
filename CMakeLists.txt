cmake_minimum_required(VERSION 2.8)

project(viewer)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11")

find_package(Qt4 REQUIRED)
#add_subdirectory(src)
#add_subdirectory(test)

set(untitled2_SOURCES 
  main.cpp
  QRootCanvas.cpp
  QRootDialog.cpp
  QRootApplication.cpp
  QRootWindow.cpp
  mainwindow.cpp
  ConnectServer.cpp 
  HistogramList.cpp 
  ContentRequestHandler.cpp 
  SpectrumViewer.cpp 
  GlobalSettings.cpp 
  HistogramView.cpp 
  ListRequestHandler.cpp 
  TGo4CreateNewHistogram.cpp 
  ParameterList.cpp 
  Compression.cpp 
  DockableGateManager.cpp 
  GateBuilderDialog.cpp 
  GateList.cpp 
  HistogramBundle.cpp 
  GateBuilder1DDialog.cpp 
  SliceTableItem.cpp 
  GSlice.cpp 
  SpecTclInterface.cpp 
  SpecTclRESTInterface.cpp 
  GateEditComHandler.cpp 
  GateEditRequest.cpp)

set(untitled2_HEADERS 
  QRootCanvas.h 
  QRootDialog.h 
  QRootApplication.h 
  QRootWindow.h 
  mainwindow.h 
  ConnectServer.h 
  SpectrumViewer.h 
  Singleton.h  
  HistogramList.h 
  GlobalSettings.h 
  ContentRequestHandler.h 
  HistogramView.h 
  ListRequestHandler.h 
  TGo4CreateNewHistogram.h 
  ParameterList.h 
  Compression.h 
  GlobalEnum.h 
  DockableGateManager.h 
  GateBuilderDialog.h 
  HistogramBundle.h 
  GateList.h 
  GateBuilder1DDialog.h 
  SliceTableItem.h 
  GSlice.h 
  SpecTclInterface.h 
  SpecTclRESTInterface.h 
  GateEditComHandler.h 
  GateEditRequest.h)

set(untitled2_FORMS
  mainwindow.ui 
  ConnectServer.ui 
  SpectrumViewer.ui 
  HistogramList.ui 
  HistogramView.ui 
  TGo4CreateNewHistogram.ui 
  DockableGateManager.ui 
  GateBuilderDialog.ui 
  GateBuilder1DDialog.ui)

set(untitled2_RESOURCES
    resources.qrc)

set(QT_USE_QTNETWORK TRUE)

# buld mocs, uis and rccs
qt4_wrap_cpp(untitled2_HEADERS_MOC ${untitled2_HEADERS})
qt4_wrap_ui(untitled2_FORMS_HEADERS ${untitled2_FORMS})
qt4_add_resources(untitled2_RESOURCES ${untitled2_RESOURCES})

# include ROOT stuff
set(ROOTCONFIGPATH $ENV{ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --cflags OUTPUT_VARIABLE _rootCFlags)
execute_process(COMMAND ${ROOTCONFIGPATH} --glibs OUTPUT_VARIABLE _rootLdFlags RESULT_VARIABLE _res)

# get rid of trailing and leading whitespace
string(STRIP ${_rootCFlags} ROOT_CXXFLAGS)
string(STRIP ${_rootLdFlags} ROOT_LDFLAGS)

# add the include paths
add_definitions(${ROOT_CXXFLAGS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# add the SpecTclJson to this
include(ExternalProject)
set(USE_PROJECT_CMAKE_MODULE_PATH "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
ExternalProject_ADD(SpecTclJson
  GIT_REPOSITORY  http://github.com/jrtomps/SpecTclJson.git
  UPDATE_COMMAND ""
  TEST_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    ${USE_PROJECT_CMAKE_MODULE_PATH}
)

set(SPECTCLJSON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include) 
set(SPECTCLJSON_LIBRARY_DIRS ${CMAKE_BINARY_DIR}/lib)

add_library(libSpecTclJson UNKNOWN IMPORTED)
set_property(TARGET libSpecTclJson PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libLibSpecTclJson.a)

include(${CMAKE_SOURCE_DIR}/FindPkgConfig.cmake)
pkg_search_module(LibSpecTclJson jsoncpp)

add_executable(untitled2 
  ${untitled2_SOURCES} 
  ${untitled2_HEADERS_MOC}
  ${untitled2_FORMS_HEADERS}
  ${untitled2_RESOURCES}) 
add_dependencies(untitled2 SpecTclJson)

# we depend on some external libraries
target_link_libraries(untitled2 
  ${QT_LIBRARIES}
  libSpecTclJson
  jsoncpp
  ${ROOT_LDFLAGS} -lQtGSI)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${SPECTCLJSON_INCLUDE_DIRS})


#GateEditRequestTest.cpp 




