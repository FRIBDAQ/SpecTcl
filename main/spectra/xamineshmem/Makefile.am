

lib_LTLIBRARIES = libXamineToRoot.la

x2rdir=@incdir@/x2r
x2r_INCLUDES = HistFiller.h

libXamineToRoot_la_SOURCES = HistFiller.cpp

libXamineToRoot_la_CXXFLAGS = @ROOT_CFLAGS@ \
                              -I@top_srcdir@/Display

libXamineToRoot_la_LIBADD = @ROOT_LDFLAGS@ \
                            @top_builddir@/Display/libXamine.la
