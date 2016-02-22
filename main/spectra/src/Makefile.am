
lib_LTLIBRARIES = libSpectra.la

bin_PROGRAMS = spectra

noinst_HEADERS =  QRootCanvas.h  \
  QRootDialog.h  \
  QRootApplication.h  \
  QRootWindow.h  \
  mainwindow.h  \
  ConnectServer.h  \
  SpectrumViewer.h  \
  Singleton.h   \
  HistogramList.h  \
  GlobalSettings.h  \
  ContentRequestHandler.h  \
  HistogramView.h  \
  ListRequestHandler.h  \
  TGo4CreateNewHistogram.h  \
  Compression.h  \
  GlobalEnum.h  \
  DockableGateManager.h  \
  GateBuilderDialog.h  \
  HistogramBundle.h  \
  GateList.h  \
  GateBuilder1DDialog.h  \
  SliceTableItem.h  \
  GateListItem.h \
  GSlice.h \
  GGate.h \
  SpecTclInterface.h  \
  SpecTclRESTInterface.h  \
  GateEditComHandler.h  \
  GateEditRequest.h \
  GateDeleteRequest.h \
  CommonResponseHandler.h \
  GateListRequestHandler.h \
  Utility.h \
  QTLine.h \
  QHistInfo.h \
  ControlPanel.h \
  GeometrySelector.h \
  SpectrumView.h \
  MultiSpectrumView.h \
  ConnectionTester.h \
  TabbedMultiSpectrumView.h 

libSpectra_la_SOURCES = \
  QRootCanvas.cpp \
  QRootDialog.cpp \
  QRootApplication.cpp \
  QRootWindow.cpp \
  mainwindow.cpp \
  ConnectServer.cpp  \
  HistogramList.cpp  \
  ContentRequestHandler.cpp  \
  SpectrumViewer.cpp  \
  GlobalSettings.cpp  \
  HistogramView.cpp  \
  ListRequestHandler.cpp  \
  TGo4CreateNewHistogram.cpp  \
  Compression.cpp  \
  DockableGateManager.cpp  \
  GateBuilderDialog.cpp  \
  GateList.cpp  \
  HistogramBundle.cpp  \
  GateBuilder1DDialog.cpp  \
  SliceTableItem.cpp  \
  GateListItem.cpp \
  GSlice.cpp  \
  GGate.cpp \
  SpecTclInterface.cpp  \
  SpecTclRESTInterface.cpp  \
  GateEditComHandler.cpp  \
  GateEditRequest.cpp \
  GateDeleteRequest.cpp \
  CommonResponseHandler.cpp \
  GateListRequestHandler.cpp \
  QTLine.cpp \
  ControlPanel.cpp \
  GeometrySelector.cpp \
  SpectrumView.cpp \
  MultiSpectrumView.cpp \
  ConnectionTester.cpp \
  TabbedMultiSpectrumView.cpp \
	moc_QRootCanvas.cpp \
	moc_QRootDialog.cpp \
	moc_QRootWindow.cpp \
	moc_QRootApplication.cpp \
	moc_mainwindow.cpp \
	moc_ConnectServer.cpp \
	moc_SpectrumViewer.cpp \
  moc_HistogramList.cpp \
	moc_HistogramView.cpp \
	moc_ContentRequestHandler.cpp \
	moc_ListRequestHandler.cpp \
	moc_TGo4CreateNewHistogram.cpp \
	moc_DockableGateManager.cpp \
	moc_GateBuilderDialog.cpp \
	moc_GateBuilder1DDialog.cpp \
  moc_GSlice.cpp \
  moc_GGate.cpp \
  moc_SpecTclInterface.cpp  \
  moc_SpecTclRESTInterface.cpp  \
  moc_GateEditComHandler.cpp \
	moc_GateEditRequest.cpp \
  moc_CommonResponseHandler.cpp \
  moc_GateListRequestHandler.cpp \
  moc_QTLine.cpp \
  moc_ControlPanel.cpp \
	moc_GeometrySelector.cpp \
	moc_MultiSpectrumView.cpp \
  moc_ConnectionTester.cpp \
	moc_TabbedMultiSpectrumView.cpp \
	qrc_resources.cpp

spectraFORMS =  mainwindow.ui  \
  ConnectServer.ui  \
  SpectrumViewer.ui  \
  HistogramList.ui  \
  HistogramView.ui  \
  TGo4CreateNewHistogram.ui  \
  DockableGateManager.ui  \
  GateBuilderDialog.ui  \
  GateBuilder1DDialog.ui \
  ControlPanel.ui \
  GeometrySelector.ui \
  TabbedMultiSpectrumView.ui

spectraRESOURCES = resources.qrc

BUILT_SOURCES = ui_mainwindow.h \
								ui_ConnectServer.h \
								ui_SpectrumViewer.h \
								ui_HistogramList.h \
								ui_HistogramView.h \
								ui_TGo4CreateNewHistogram.h \
								ui_DockableGateManager.h \
								ui_GateBuilderDialog.h \
								ui_GateBuilder1DDialog.h \
								ui_ControlPanel.h \
								ui_GeometrySelector.h \
								ui_TabbedMultiSpectrumView.h

libSpectra_la_CXXFLAGS = @ROOT_CFLAGS@ \
									 -I@top_srcdir@/spectra/spectcljson \
									 -I@top_srcdir@/spectra/jsoncpp/include

libSpectra_la_LIBADD = @ROOT_LDFLAGS@ -lQtGSI \
								@top_builddir@/spectra/spectcljson/libSpecTclJson.la \
								@top_builddir@/spectra/jsoncpp/src/lib_json/libjsoncpp.la \
								-lz -lX11

moc_%.cpp : %.h
		@MOC@ -o$@ $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(MOC_CPPFLAGS) $<

ui_%.h : %.ui
	@UIC@ -o $@ $<

qrc_%.cpp : %.qrc
	@RCC@ -name $(basename $<) -o $@ $<


spectra_SOURCES = main.cpp

spectra_LDADD = @top_builddir@/spectra/src/libSpectra.la \
								@ROOT_LDFLAGS@ -lQtGSI \
								@top_builddir@/spectra/spectcljson/libSpecTclJson.la \
								@top_builddir@/spectra/jsoncpp/src/lib_json/libjsoncpp.la \
								-lz -lX11
spectra_CXXFLAGS =  @ROOT_CFLAGS@ \
									 -I@top_srcdir@/spectra/spectcljson \
									 -I@top_srcdir@/spectra/jsoncpp/include


