

lib_LTLIBRARIES = libXamine2Root.la

x2rdir=@incdir@/Xamine2Root
x2r_INCLUDES = HistFiller.h

libXamine2Root_la_SOURCES = HistFiller.cpp \
                              @top_srcdir@/Display/dispshare.h \
                              @top_srcdir@/Display/spectra.cc \
                              @top_srcdir@/Display/shared.cc

libXamine2Root_la_CXXFLAGS = @ROOT_CFLAGS@ \
                              -I@top_srcdir@/Display

libXamine2Root_la_LIBADD = @ROOT_LDFLAGS@ \
                            @top_builddir@/Display/libXamine.la

bin_PROGRAMS = unittests

unittests_SOURCES = TestRunner.cpp HistFillerTest.cpp
unittests_CXXFLAGS = @ROOT_CFLAGS@ \
										 -I@top_srcdir@/Display \
										 @CPPUNIT_CFLAGS@ \
										 -I@top_srcdir@/Utility

unittests_LDADD = @ROOT_LDFLAGS@ \
										@top_builddir@/Display/libXamine.la \
										@builddir@/libXamine2Root.la \
										@CPPUNIT_LIBS@ \
										@TCL_LIBS@ 
unittests_LDFLAGS = -Wl,-rpath=@ROOT_LIBRARY_DIR@

TESTS = unittests
