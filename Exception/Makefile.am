INCLUDES = -I. -I../Utility -I../Exception
LDADD = -lXplus -lXamine
LDFLAGS = -L.

lib_LTLIBRARIES = libException.la

libException_la_LDFLAGS = -version-info $(SOVERSION):0

libException_la_SOURCES = ErrnoException.cpp Exception.cpp RangeError.cpp \
	StreamIOError.cpp

include_HEADERS = ErrnoException.h Exception.h RangeError.h StreamIOError.h

EXTRA_DIST = Exception.WC Exception.omt
