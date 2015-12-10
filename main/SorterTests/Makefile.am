AM_CPPFLAGS = -I. -I../Utility @LIBTCLPLUS_CFLAGS@ -I../Xamine -I../Gates \
	   -I../Events -I../Display @TCL_CPPFLAGS@ -I.. -I@top_srcdir@/Fits 

if CPPUNIT
noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp TestHistogrammer.cpp

TestRunner_CXXFLAGS=@CPPUNIT_CFLAGS@

TestRunner_LDADD = @CPPUNIT_LIBS@ @top_builddir@/Sorter/libSorting.la \
									 @top_builddir@/Sorter/libtape.la

noinst_HEADERS = Asserts.h

TESTS=./TestRunner

endif
