

lib_LTLIBRARIES = libXamineToRoot.la

x2rdir=@incdir@/x2r
x2r_INCLUDES = HistFiller.h

libXamineToRoot_la_SOURCES = HistFiller.cpp ../../Display/dispshare.h ../../Display/spectra.cc ../../Display/shared.cc

libXamineToRoot_la_CXXFLAGS = @ROOT_CFLAGS@ \
                              -I@top_srcdir@/Display

libXamineToRoot_la_LIBADD = @ROOT_LDFLAGS@ \
                            @top_builddir@/Display/libXamine.la

bin_PROGRAMS = unittests

unittests_SOURCES = TestRunner.cpp HistFillerTest.cpp
unittests_CXXFLAGS = @ROOT_CFLAGS@ \
										 -I@top_srcdir@/Display \
										 @CPPUNIT_CFLAGS@ \
										 -I@top_srcdir@/Utility

unittests_LDADD = @ROOT_LDFLAGS@ \
										@top_builddir@/Display/libXamine.la \
										@builddir@/libXamineToRoot.la \
										@CPPUNIT_LIBS@ \
										@TCL_LIBS@ 
unittests_LDFLAGS = -Wl,-rpath=@ROOT_LIBRARY_DIR@

TESTS = unittests
