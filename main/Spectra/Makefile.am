
lib_LTLIBRARIES = libSpectraServer.la


libSpectraServer_la_SOURCES = SpectraLocalDisplay.cpp SpectraProcess.cpp

include_HEADERS = SpectraLocalDisplay.h SpectraProcess.h

libSpectraServer_la_CXXFLAGS = -I@top_srcdir@/Utility \
                            -I@top_srcdir@/Display \
                            -I@top_srcdir@/DisplayInterface \
                            -I@top_srcdir@/Sorter \
                            -I@top_srcdir@/Gates \
                            -I@top_srcdir@/Fits \
                            -I@top_srcdir@/Xamine \
                            @LIBTCLPLUS_CFLAGS@

libSpectraServer_la_LIBADD = @top_builddir@/DisplayInterface/libDisplayInterface.la \
                          @top_builddir@/Xamine/libXplus.la \
                          @top_builddir@/Display/libXamine.la \
                          @top_builddir@/Sorter/libSorting.la \
                          @top_builddir@/Events/libAnalysis.la \
                          @top_builddir@/Gates/libGates.la \
                          @top_builddir@/Fits/libFits.la \
                          @top_builddir@/Tape/libtape.la \
                          @LIBTCLPLUS_LDFLAGS@
