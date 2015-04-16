### untitled2 executable

set(untitled2_SOURCES 
  #  main.cpp
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
  Compression.cpp 
  DockableGateManager.cpp 
  GateBuilderDialog.cpp 
  GateList.cpp 
  HistogramBundle.cpp 
  GateBuilder1DDialog.cpp 
  SliceTableItem.cpp 
  GateListItem.cpp
  GSlice.cpp 
  GGate.cpp
  SpecTclInterface.cpp 
  SpecTclRESTInterface.cpp 
  GateEditComHandler.cpp 
  GateEditRequest.cpp
  GateDeleteRequest.cpp
  CommonResponseHandler.cpp
  GateListRequestHandler.cpp)

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
  Compression.h 
  GlobalEnum.h 
  DockableGateManager.h 
  GateBuilderDialog.h 
  HistogramBundle.h 
  GateList.h 
  GateBuilder1DDialog.h 
  SliceTableItem.h 
  GateListItem.h
  GSlice.h
  GGate.h
  SpecTclInterface.h 
  SpecTclRESTInterface.h 
  GateEditComHandler.h 
  GateEditRequest.h
  GateDeleteRequest.h
  CommonResponseHandler.h
  GateListRequestHandler.h)

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

## ------ QT BUILDING STUFF --------------------------------------------
#
# Module enabling
# uis, mocs, rccs

set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTTEST TRUE)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})


# buld mocs, uis and rccs
qt4_wrap_cpp(untitled2_HEADERS_MOC ${untitled2_HEADERS})
qt4_wrap_ui(untitled2_FORMS_HEADERS ${untitled2_FORMS})
qt4_add_resources(untitled2_RESOURCES ${untitled2_RESOURCES})


## ----   SPECTCLJSON ------------------------------------------------- 

# add the SpecTclJson to this
include(ExternalProject)
set(USE_PROJECT_CMAKE_MODULE_PATH "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
ExternalProject_ADD(SpecTclJson
  GIT_REPOSITORY  http://github.com/jrtomps/SpecTclJson.git
  GIT_TAG master
  #  UPDATE_COMMAND ${GIT_EXECUTABLE} pull
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
set_property(TARGET libSpecTclJson 
             PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libLibSpecTclJson.a)

#### ----- ROOT -------------------------------------------------------- 

set(ROOTCONFIGPATH $ENV{ROOTSYS}/bin/root-config)
execute_process(COMMAND ${ROOTCONFIGPATH} --cflags OUTPUT_VARIABLE _rootCFlags)
execute_process(COMMAND ${ROOTCONFIGPATH} --glibs OUTPUT_VARIABLE _rootLdFlags RESULT_VARIABLE _res)

# get rid of trailing and leading whitespace
string(STRIP ${_rootCFlags} ROOT_CXXFLAGS)
string(STRIP ${_rootLdFlags} ROOT_LDFLAGS)

# add the include paths
add_definitions(${ROOT_CXXFLAGS})


## ------- JSONCPP -----------------------------------------------------


pkg_search_module(LibSpecTclJson jsoncpp)




include_directories(${CMAKE_CURRENT_BINARY_DIR}
                     ${SPECTCLJSON_INCLUDE_DIRS})

########################################################################

# ADD EXECUTABLE
add_library(Untitled2 
  ${untitled2_SOURCES} 
  ${untitled2_HEADERS_MOC}
  ${untitled2_FORMS_HEADERS}
  ${untitled2_RESOURCES}) 

add_dependencies(Untitled2 SpecTclJson)

# we depend on some external libraries
target_link_libraries(Untitled2 
  ${QT_LIBRARIES}
  libSpecTclJson
  jsoncpp
  ${ROOT_LDFLAGS} -lQtGSI)


add_executable(untitled2 main.cpp)
target_link_libraries(untitled2 Untitled2)


#### TESTLIB .... sample
#set(test2_SOURCES testqstring.cpp)
#set(test2_HEADERS testqstring.h)
#qt4_wrap_cpp(test2_HEADERS_MOC ${test2_HEADERS})
#add_executable(test2 ${test2_SOURCES} ${test2_HEADERS_MOC})
#target_link_libraries(test2  ${QT_LIBRARIES})
#
#add_test(TEST2 test2)
