DEFS = @WCHAR_T_DEF@
INCLUDES = -I@top_srcdir@ -I. -I@top_srcdir@/Utility \
	-I@top_srcdir@/NSCLException -I@top_srcdir@/EventSource \
	-I@top_srcdir@/TCL \
	-I@top_srcdir@/SpecTcl  -I@top_srcdir@/Filter \
        -I@top_srcdir@/Xamine \
        -I@top_srcdir@/Sorter \
        -I@top_srcdir@/factories \
        -I@top_srcdir@/Display    \
        -I@top_srcdir@/Gates \
	@TCL_CPPFLAGS@ @TK_CPPFLAGS@ @XINCLUDES@ 
LDADD = @TK_LIBS@ @TCL_LIBS@


lib_LTLIBRARIES = libAnalysis.la
libAnalysis_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libAnalysis_la_SOURCES = Analyzer.cpp BufferDecoder.cpp BufferTranslator.cpp \
	Event.cpp EventFormatError.cpp EventList.cpp NSCLBufferDecoder.cpp \
	RunControl.cpp TCLRunFileHandler.cpp TKRunControl.cpp EventSink.cpp \
	EventSinkPipeline.cpp NSCLJumboBufferDecoder.cpp DopeVector.cpp

include_HEADERS = Analyzer.h BufferDecoder.h BufferTranslator.h Event.h \
	EventFormatError.h EventList.h EventSink.h EventUnpacker.h \
	NSCLBufferDecoder.h RunControl.h TCLRunFileHandler.h TKRunControl.h \
	TranslatorPointer.h ValidValue.h EventSinkPipeline.h \
	NSCLJumboBufferDecoder.h DopeVector.h

EXTRA_DIST = ClassBreakdown.WC ClassBreakdown.omt
