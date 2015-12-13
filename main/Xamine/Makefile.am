AM_CPPFLAGS = -g

lib_LTLIBRARIES = libXplus.la

libXplus_la_CPPFLAGS = -I@srcdir@ \
			-I@top_srcdir@/Utility \
			-I@top_srcdir@/Display \
			-I@top_srcdir@/Sorter \
			-I@top_srcdir@/Events \
			-I@top_srcdir@/Gates \
			-I@top_srcdir@/Fits \
			-I@top_srcdir@/Filter \
			@TCL_CPPFLAGS@ \
			@LIBTCLPLUS_CFLAGS@

libXplus_la_LIBADD =	@top_builddir@/Display/libXamine.la \
											@top_builddir@/Sorter/libSorting.la \
											@top_builddir@/Events/libAnalysis.la \
											@top_builddir@/Gates/libGates.la \
											@top_builddir@/Fits/libFits.la \
											@top_builddir@/Filter/libFilter.la


libXplus_la_LDFLAGS = -verson-info $(SOVERSION):0 \
	-Wl,"-rpath=$(libdir)" \
	@TCL_LIBS@ \
	@LIBTCLPLUS_LDFLAGS@

libXplus_la_SOURCES = DisplayGate.cpp Display.cpp DisplayManager.cpp \
	Xamine1D.cpp Xamine2D.cpp \
	XamineButton.cpp XamineButtonException.cpp XamineButtonPrompt.cpp \
	XamineConfirmPrompt.cpp XamineEvent.cpp XamineFilePrompt.cpp \
	XamineGateException.cpp XamineGates.cpp \
	XamineNoPrompt.cpp XaminePointsPrompt.cpp \
	XamineSpectrumPrompt.cpp  XaminePushButton.cpp \
	XamineSpectrum.cpp XamineSpectrumIterator.cpp \
	XamineTextPrompt.cpp XamineToggleButton.cpp Xamineplus.cpp

include_HEADERS = ButtonEvent.h DisplayGate.h Display.h DisplayManager.h \
	Point.h Xamine1D.h Xamine2D.h \
	XamineButton.h XamineButtonException.h XamineButtonPrompt.h \
	XamineConfirmPrompt.h XamineEvent.h XamineFilePrompt.h \
	XamineGateException.h XamineGates.h XamineMap1D.h XamineMap2D.h \
	XamineNoPrompt.h XaminePointsPrompt.h XaminePushButton.h \
	XamineSpectrum.h XamineSpectrumIterator.h XamineSpectrumPrompt.h \
	XamineTextPrompt.h XamineToggleButton.h Xamineplus.h

EXTRA_DIST = ClassBreakdown.WC ClassBreakdown.omt

noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp DisplayManagerTests.cpp

TestRunner_CXXFLAGS=-I@top_srcdir@/Events \
		    -I@top_srcdir@/Utility \
		    @LIBTCLPLUS_CFLAGS@ \
		    @CPPUNIT_CFLAGS@

TestRunner_LDADD = @top_builddir@/Xamine/libXplus.la \
		    @top_builddir@/Sorter/libSorting.la \
		    @top_builddir@/Fits/libFits.la \
				@top_builddir@/Display/libXamine.la \
		    @top_builddir@/Tape/libtape.la \
		    @CPPUNIT_LIBS@ \
		    @TCL_LIBS@

noinst_HEADERS = Asserts.h

check-TESTS:
	./TestRunner



