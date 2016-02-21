### spectra executable

set(spectra_SOURCES
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
  GateListRequestHandler.cpp
  QTLine.cpp
  ControlPanel.cpp
  GeometrySelector.cpp
  SpectrumView.cpp
  MultiSpectrumView.cpp
  ConnectionTester.cpp
  TabbedMultiSpectrumView.cpp)

set(spectra_HEADERS
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
  GateListRequestHandler.h
  Utility.h
  QTLine.h
  QHistInfo.h
  ControlPanel.h
  GeometrySelector.h
  SpectrumView.h
  MultiSpectrumView.h
  ConnectionTester.h
  TabbedMultiSpectrumView.h)

set(spectra_FORMS
  mainwindow.ui 
  ConnectServer.ui 
  SpectrumViewer.ui 
  HistogramList.ui 
  HistogramView.ui 
  TGo4CreateNewHistogram.ui 
  DockableGateManager.ui 
  GateBuilderDialog.ui 
  GateBuilder1DDialog.ui
  ControlPanel.ui
  GeometrySelector.ui
  TabbedMultiSpectrumView.ui)

set(spectra_RESOURCES
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
qt4_wrap_cpp(spectra_HEADERS_MOC ${spectra_HEADERS})
qt4_wrap_ui(spectra_FORMS_HEADERS ${spectra_FORMS})
qt4_add_resources(spectra_RESOURCES ${spectra_RESOURCES})


## ------- JSONCPP -----------------------------------------------------
pkg_search_module(JSONCPP jsoncpp)

add_definitions(${ROOT_CXXFLAGS})

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${JSONCPP_INCLUDE_DIRS}
                    ${SPECTCLJSON_INCLUDE_DIRS}
                    ${ROOT_INCLUDE_DIRS})

########################################################################

# ADD EXECUTABLE
add_library(Spectra
  ${spectra_SOURCES}
  ${spectra_HEADERS_MOC}
  ${spectra_FORMS_HEADERS}
  ${spectra_RESOURCES})

add_dependencies(Spectra SpecTclJson)

# we depend on some external libraries
target_link_libraries(Spectra
  ${QT_LIBRARIES}
  LibSpecTclJson
  ${JSONCPP_LIBRARIES}
  ${ROOT_LDFLAGS} -lQtGSI)


add_executable(spectra main.cpp)
target_link_libraries(spectra Spectra ROOT)


#### TESTLIB .... sample
#set(test2_SOURCES testqstring.cpp)
#set(test2_HEADERS testqstring.h)
#qt4_wrap_cpp(test2_HEADERS_MOC ${test2_HEADERS})
#add_executable(test2 ${test2_SOURCES} ${test2_HEADERS_MOC})
#target_link_libraries(test2  ${QT_LIBRARIES})
#
#add_test(TEST2 test2)

install(TARGETS spectra DESTINATION bin)
install(TARGETS Spectra DESTINATION lib)
