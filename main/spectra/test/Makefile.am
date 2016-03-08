
bin_PROGRAMS = unittests

unittests_SOURCES = DockableGateManagerTest.cpp \
										GGateTest.cpp \
										GSliceTest.cpp \
										GateDeleteRequestTest.cpp \
										GateEditRequestTest.cpp \
										GateListTest.cpp \
										HistogramBundleTest.cpp \
										HistogramListTest.cpp \
										QTLineTest.cpp \
										TestRunner.cpp \
										MainWindowTest.cpp \
										CanvasOpsTest.cpp \
										ShMemKeyRequestHandlerTest.cpp \
										SpecTclInterfaceFactoryTest.cpp \
										TestUtils.cpp

noinst_HEADERS = TestUtils.h

unittests_LDFLAGS = @ROOT_LDFLAGS@ \
										 @JSONCPP_LIBS@ \
										 @top_builddir@/spectra/src/libSpectra.la \
										 @top_builddir@/spectra/spectcljson/libSpecTclJson.la \
										 @CPPUNIT_LIBS@ \
										 -Wl,-rpath=@ROOT_LIBRARY_DIR@

unittests_CXXFLAGS = @ROOT_CFLAGS@ \
										 @JSONCPP_CFLAGS@ \
										 -I@top_srcdir@/spectra/src \
										 -I@top_srcdir@/spectra/spectcljson \
										 @CPPUNIT_CFLAGS@ \
										 -I@top_srcdir@/Utility


TESTS = unittests
