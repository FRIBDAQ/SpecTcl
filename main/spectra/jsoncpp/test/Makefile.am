

noinst_PROGRAMS = spectratest

spectratest_SOURCES = TestRunner.cpp \
  GateEditRequestTest.cpp \
  GateDeleteRequestTest.cpp \
  GSliceTest.cpp \
  GGateTest.cpp \
  HistogramListTest.cpp \
  GateListTest.cpp \
  DockableGateManagerTest.cpp \
  QTLineTest.cpp \
  HistogramBundleTest.cpp

spectratest_CXXFLAGS = -I@top_srcdir@/spectra/src  \
											 @JSONCPP_CXXFLAGS@

spectratest_LDADD = @top_builddir@/spectra/src/libSpectra.la \
										@JSONCPP_LDFLAGS@ 
