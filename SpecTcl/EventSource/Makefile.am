INCLUDES = -I. -I../Utility -I../Exception  -I../Tape

lib_LTLIBRARIES = libEventSource.la

libEventSource_la_SOURCES = File.cpp GaussianDistribution.cpp \
	TapeException.cpp TapeFile.cpp TestFile.cpp PipeFile.cpp

include_HEADERS = DiskFile.h Distribution.h File.h GaussianDistribution.h \
	PipeFile.h TapeException.h TapeFile.h TestFile.h

EXTRA_DIST = classbreakdown.WC classbreakdown.omt
