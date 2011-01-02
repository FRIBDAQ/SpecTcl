ACLOCAL_AMFLAGS=-I m4

INCLUDES = -I. -I../Utility 

# AM_LDFLAGS = -L.

lib_LTLIBRARIES = libException.la

libException_la_LDFLAGS =  \
	-Wl,"-rpath-link=$(libdir)"

libException_la_SOURCES = ErrnoException.cpp Exception.cpp RangeError.cpp \
	StreamIOError.cpp StateException.cpp

include_HEADERS = ErrnoException.h Exception.h RangeError.h StreamIOError.h \
	StateException.h

