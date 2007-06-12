INCLUDES = -I. -I../Utility -I../NSCLException  -I../Tape -I../SpecTcl \
	-I../Events -I../TCL -I../Filter @TCL_FLAGS@


lib_LTLIBRARIES = libEventSource.la
libEventSource_la_LDFLAGS =-L. @TCL_LDFLAGS@\
	 -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libEventSource_la_SOURCES = File.cpp GaussianDistribution.cpp \
	TapeException.cpp TapeFile.cpp TestFile.cpp PipeFile.cpp  \
	MultiTestSource.cpp CalibrationDistribution.cpp

include_HEADERS = DiskFile.h Distribution.h File.h GaussianDistribution.h \
	PipeFile.h TapeException.h TapeFile.h TestFile.h  MultiTestSource.h \
	CalibrationDistribution.h

EXTRA_DIST = classbreakdown.WC classbreakdown.omt
