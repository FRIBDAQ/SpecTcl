DEFS = @WCHAR_T_DEF@
INCLUDES = -I@top_srcdir@ -I. -I../Utility -I../NSCLException -I../EventSource -I../TCL \
	-I../SpecTcl -I../Filter \
	@TCL_FLAGS@ @XINCLUDES@

LDFLAGS = @TCL_LDFLAGS@

lib_LTLIBRARIES = libAnalysis.la
libAnalysis_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libAnalysis_la_SOURCES = Analyzer.cpp BufferDecoder.cpp BufferTranslator.cpp \
	Event.cpp EventFormatError.cpp EventList.cpp NSCLBufferDecoder.cpp \
	RunControl.cpp TCLRunFileHandler.cpp TKRunControl.cpp EventSink.cpp

include_HEADERS = Analyzer.h BufferDecoder.h BufferTranslator.h Event.h \
	EventFormatError.h EventList.h EventSink.h EventUnpacker.h \
	NSCLBufferDecoder.h RunControl.h TCLRunFileHandler.h TKRunControl.h \
	TranslatorPointer.h ValidValue.h

EXTRA_DIST = ClassBreakdown.WC ClassBreakdown.omt
