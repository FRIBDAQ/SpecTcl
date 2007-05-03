DEFS = @WCHAR_T_DEF@
INCLUDES = -I@top_srcdir@ -I. -I@top_srcdir@/Utility \
	-I@top_srcdir@/NSCLException -I@top_srcdir@/EventSource \
	-I@top_srcdir@/TCL \
	-I@top_srcdir@/SpecTcl  -I@top_srcdir@/Filter \
	@TCL_FLAGS@ @XINCLUDES@ 
LDADD = @TCL_LDFLAGS@
# AM_LDFLAGS = @TCL_LDFLAGS@

lib_LTLIBRARIES = libAnalysis.la
libAnalysis_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libAnalysis_la_SOURCES = Analyzer.cpp BufferDecoder.cpp BufferTranslator.cpp \
	Event.cpp EventFormatError.cpp EventList.cpp NSCLBufferDecoder.cpp \
	RunControl.cpp TCLRunFileHandler.cpp TKRunControl.cpp EventSink.cpp \
	EventSinkPipeline.cpp NSCLJumboBufferDecoder.cpp

include_HEADERS = Analyzer.h BufferDecoder.h BufferTranslator.h Event.h \
	EventFormatError.h EventList.h EventSink.h EventUnpacker.h \
	NSCLBufferDecoder.h RunControl.h TCLRunFileHandler.h TKRunControl.h \
	TranslatorPointer.h ValidValue.h EventSinkPipeline.h \
	NSCLJumboBufferDecoder.cpp

EXTRA_DIST = ClassBreakdown.WC ClassBreakdown.omt
