
noinst_PROGRAMS = unittests

unittests_SOURCES = GGateTest.cpp \
		TestSpecTclInterface.h \
		TestSpecTclInterface.cpp \
                MainTest.cpp \
		TestRunner.cpp \
		RootFileWriterTest.cpp 
		GSliceTest.cpp \
		GateDeleteRequestTest.cpp \
		GateEditRequestTest.cpp \
		GateListTest.cpp \
		HistogramBundleTest.cpp \
		HistogramListTest.cpp \
		QTLineTest.cpp \
		MainWindowTest.cpp \
		CanvasOpsTest.cpp \
		ShMemKeyRequestHandlerTest.cpp \
		SpecTclInterfaceFactoryTest.cpp \
		CmdLineOptionsTest.cpp \
		PrintingSystemTests.cpp \
		WinFileWriterTest.cpp \
		SubscribableH1Test.cpp \
		TestUtils.cpp





noinst_HEADERS = TestUtils.h

unittests_LDFLAGS =  -Wl,-rpath=@ROOT_LIBRARY_DIR@

unittests_LDADD = @ROOT_LDFLAGS@ \
					   @JSONCPP_LIBS@ \
										 @top_builddir@/spectra/src/libSpectra.la \
										 @top_builddir@/spectra/spectcljson/libSpecTclJson.la \
										 @top_builddir@/Display/libXamine.la \
										 @top_builddir@/XamineParsers/libXamineParsers.la \
										 @top_builddir@/spectra/Xamine2Root/libXamine2Root.la \
										 @CPPUNIT_LIBS@

unittests_CXXFLAGS = @ROOT_CFLAGS@ \
										 @JSONCPP_CFLAGS@ \
										 -I@top_srcdir@/spectra/src \
										 -I@top_srcdir@/spectra/spectcljson \
										 -I@top_srcdir@/XamineParsers \
										 -I@top_srcdir@/Display \
										 @CPPUNIT_CFLAGS@ \
										 -I@top_srcdir@/Utility


TESTS = unittests
