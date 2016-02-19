

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
  TabbedMultiSpectrumView.h \
  Benchmarker.h

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
  Benchmarker.cpp \
	moc_QRootCanvas.cpp  \
  moc_QRootDialog.cpp  \
  moc_QRootApplication.cpp  \
  moc_QRootWindow.cpp  \
  moc_mainwindow.cpp  \
  moc_ConnectServer.cpp  \
  moc_SpectrumViewer.cpp  \
  moc_Singleton.cpp   \
  moc_HistogramList.cpp  \
  moc_GlobalSettings.cpp  \
  moc_ContentRequestHandler.cpp  \
  moc_HistogramView.cpp  \
  moc_ListRequestHandler.cpp  \
  moc_TGo4CreateNewHistogram.cpp  \
  moc_Compression.cpp  \
  moc_GlobalEnum.cpp  \
  moc_DockableGateManager.cpp  \
  moc_GateBuilderDialog.cpp  \
  moc_HistogramBundle.cpp  \
  moc_GateList.cpp  \
  moc_GateBuilder1DDialog.cpp  \
  moc_SliceTableItem.cpp  \
  moc_GateListItem.cpp \
  moc_GSlice.cpp \
  moc_GGate.cpp \
  moc_SpecTclInterface.cpp  \
  moc_SpecTclRESTInterface.cpp  \
  moc_GateEditComHandler.cpp  \
  moc_GateEditRequest.cpp \
  moc_GateDeleteRequest.cpp \
  moc_CommonResponseHandler.cpp \
  moc_GateListRequestHandler.cpp \
  moc_Utility.cpp \
  moc_QTLine.cpp \
  moc_QHistInfo.cpp \
  moc_ControlPanel.cpp \
  moc_GeometrySelector.cpp \
  moc_SpectrumView.cpp \
  moc_MultiSpectrumView.cpp \
  moc_ConnectionTester.cpp \
  moc_TabbedMultiSpectrumView.cpp \
  moc_Benchmarker.cpp \
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
								TabbedMultiSpectrumView.h

spectra_CXXFLAGS = @ROOT_CFLAGS@
spectra_LDADD = @ROOT_LDFLAGS@

moc_%.cpp : %.h
		@MOC -i -o $@ $<

ui_%.h : %.ui
	@UIC@ -o $@ $<

qrc_%.cpp : %.qrc
	@RCC@ -o $@ $<

