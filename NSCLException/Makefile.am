INCLUDES = -I. -I../Utility -I../Exception
LDADD = -lXplus -lXamine -L.
# AM_LDFLAGS = -L.

lib_LTLIBRARIES = libException.la

libException_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libException_la_SOURCES = ErrnoException.cpp Exception.cpp RangeError.cpp \
	StreamIOError.cpp StateException.cpp

include_HEADERS = ErrnoException.h Exception.h RangeError.h StreamIOError.h \
	StateException.h

EXTRA_DIST = Exception.WC Exception.omt
