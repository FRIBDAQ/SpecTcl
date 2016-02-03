AM_CPPFLAGS = -g

lib_LTLIBRARIES = libDisplayInterface.la

libDisplayInterface_la_CPPFLAGS = -I@srcdir@ \
				-I@top_srcdir@/Sorter \
				-I@top_srcdir@/Gates \
				-I@top_srcdir@/Events \
				-I@top_srcdir@/Utility \
				-I@top_srcdir@/Fits \
				@TCL_CPPFLAGS@ \
				@LIBTCLPLUS_CFLAGS@

libDisplayInterface_la_LIBADD = @top_builddir@/Sorter/libSorting.la \
				@top_builddir@/Gates/libGates.la \
				@top_builddir@/Events/libAnalysis.la \
				@top_builddir@/Fits/libFits.la


libDisplayInterface_la_LDFLAGS = -verson-info $(SOVERSION):0 \
	-Wl,"-rpath=$(libdir)" \
	@TCL_LIBS@ \
	@LIBTCLPLUS_LDFLAGS@


libDisplayInterface_la_SOURCES = Display.cpp \
			DisplayFactory.cpp \
			DisplayCollection.cpp \
			DisplayInterface.cpp \
			NullDisplay.cpp \
			TestDisplay.cpp \
			CHistogrammerFitObserver.cpp \
			GateBinderObserver.cpp \
			GatingDisplayObserver.cpp

include_HEADERS = Display.h \
		    DisplayFactory.h \
		    DisplayCollection.h \
		    DisplayInterface.h \
		    NullDisplay.h \
		    TestDisplay.h \
		    CHistogrammerFitObserver.h \
		    GateBinderObserver.h \
		    GatingDisplayObserver.h \
		    Subprocess.h


noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp \
		    DisplayCollectionTests.cpp \
		    DisplayFactoryTests.cpp \
		    DisplayInterfaceTests.cpp

TestRunner_CPPFLAGS=-I@top_srcdir@/Utility \
		    -I@top_srcdir@/Events \
		    @LIBTCLPLUS_CFLAGS@ \
		    @CPPUNIT_CFLAGS@

TestRunner_LDADD = @builddir@/libDisplayInterface.la \
		    @top_builddir@/Sorter/libSorting.la \
		    @top_builddir@/Gates/libGates.la \
		    @top_builddir@/Events/libAnalysis.la \
		    @top_builddir@/EventSource/libEventSource.la \
		    @top_builddir@/Tape/libtape.la \
		    @top_builddir@/Fits/libFits.la \
		    @CPPUNIT_LIBS@ \
		    @TCL_LIBS@


check-TESTS:
	./TestRunner



