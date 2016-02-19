

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

spectra_SOURCES = \
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
  moc_GSlice.cpp \
  moc_GGate.cpp \
  moc_SpecTclInterface.cpp  \
  moc_CommonResponseHandler.cpp \
  moc_GateListRequestHandler.cpp \
  moc_QTLine.cpp \
  moc_ConnectionTester.cpp \
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

spectra_CXXFLAGS = @ROOT_CFLAGS@ \
									 -I@top_srcdir@/spectra/spectcljson

spectra_LDADD = @ROOT_LDFLAGS@ \
								@top_builddir@/spectra/spectcljson/libSpecTclJson.la

moc_%.cpp : %.h
		@MOC@ -o$@ $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(MOC_CPPFLAGS) $<

ui_%.h : %.ui
	@UIC@ -o $@ $<

qrc_%.cpp : %.qrc
	@RCC@ -o $@ $<

