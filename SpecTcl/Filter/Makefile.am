INCLUDES = -g  -I. -I@top_srcdir@/Sorter -I@top_srcdir@/Events \
	 -I@top_srcdir@/Utility -I@top_srcdir@/Gates \
	-I@top_srcdir@/SpecTcl -I@top_srcdir@/Xamine \
	-I@top_srcdir@/NSCLException -I@top_srcdir@/TCL \
	-I@top_srcdir@/EventSource -I@top_srcdir@/Display \
	@TCL_FLAGS@

AM_LDFLAGS  = -L.

lib_LTLIBRARIES = libFilter.la
libFilter_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libFilter_la_SOURCES = EventFilter.cpp \
	EventSinkPipeline.cpp \
	FilterDictionary.cpp \
	FilterBufferDecoder.cpp \
	FilterEventProcessor.cpp \
	GatedEventFilter.cpp \
	CXdrOutputStream.cpp CXdrInputStream.cpp \
	CXdrMemInputStream.cpp

include_HEADERS = EventFilter.h \
	          FilterDictionary.h  \
		  EventSinkPipeline.h \
	   	  FilterEventProcessor.h \
		  FilterBufferDecoder.h \
	 	  GatedEventFilter.h    \
		  CXdrOutputStream.h CXdrInputStream.h \
		  CXdrMemInputStream.h
