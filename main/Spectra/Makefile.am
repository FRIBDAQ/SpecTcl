
lib_LTLIBRARIES = libSpectraServer.la


libSpectraServer_la_SOURCES = SpectraDisplay.cpp

include_HEADERS = SpectraDisplay.h

libSpectraServer_la_CXXFLAGS = -I@top_srcdir@/DisplayInterface \
                            -I@top_srcdir@/Display \
                            -I@top_srcdir@/Gates \
                            -I@top_srcdir@/Sorter \
                            -I@top_srcdir@/Fits \
                            -I@top_srcdir@/Utility

libSpectraServer_la_LIBADD = @top_builddir@/DisplayInterface/libDisplayInterface.la \
                          @top_builddir@/Display/libXamine.la \
                          @top_builddir@/Gates/libGates.la \
                          @top_builddir@/Sorter/libSorting.la \
                          @top_builddir@/Fits/libFits.la
