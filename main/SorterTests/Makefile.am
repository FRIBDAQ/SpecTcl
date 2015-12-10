AM_CPPFLAGS = -I@srcdir@ -I@top_srcdir@/Sorter -I@top_srcdir@/Utility \
-I@top_srcdir@/Xamine -I@top_srcdir@/Gates \
	   -I@top_srcdir@/Events -I@top_srcdir@/Filter \
	   -I@top_srcdir@/Display -I@top_srcdir@/SpecTcl \
	   -I@top_srcdir@ -I@top_srcdir@/Fits \
	   @TCL_CPPFLAGS@ @LIBTCLPLUS_CFLAGS@


if CPPUNIT
noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp TestHistogrammer.cpp

TestRunner_CXXFLAGS=@CPPUNIT_CFLAGS@

TestRunner_LDADD = @CPPUNIT_LIBS@ \
		    @top_builddir@/Sorter/libSorting.la \
		    @top_builddir@/Fits/libFits.la \
		    @top_builddir@/Tape/libtape.la \
		    @top_builddir@/Xamine/libXplus.la

noinst_HEADERS = Asserts.h

TESTS=./TestRunner

endif
