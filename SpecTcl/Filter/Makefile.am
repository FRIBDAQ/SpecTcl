INCLUDES = -g  -I. -I../Sorter -I../Events -I../Utility -I../Gates \
	-I../SpecTcl -I../Xamine -I../Exception -I../TCL \
	-I../EventSource -I../Display \
	@TCL_FLAGS@
LDFLAGS  = -L.

lib_LTLIBRARIES = libFilter.la

libFilter_la_SOURCES = EventFilter.cpp \
	EventSinkPipeline.cpp \
	FilterDictionary.cpp \
	FilterBufferDecoder.cpp \
	FilterEventProcessor.cpp \
	GatedEventFilter.cpp \
	CXdrOutputStream.cpp CXdrInputStream.cpp

include_HEADERS = EventFilter.h \
	          FilterDictionary.h  \
		  EventSinkPipeline.h \
	   	  FilterEventProcessor.h \
		  FilterBufferDecoder.h \
	 	  GatedEventFilter.h    \
		  CXdrOutputStream.h CXdrInputStream.h
