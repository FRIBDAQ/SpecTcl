lib_LTLIBRARIES = libBufferAnalysis.la

libBufferAnalysis_la_SOURCES = CBufferCallbackList.cpp \
	CBufferProcessor.cpp         \
	CLanguageCallback.cpp        \
	CStandardControlCallback.cpp \
	CStandardScalerCallback.cpp  \
	BufferAnalysis.cpp

include_HEADERS = CBufferCallback.h \
	CBufferCallbackList.h       \
	CBufferProcessor.h          \
	CLanguageCallback.h         \
	CStandardControlCallback.h  \
	CStandardScalerCallback.h   \
	SRunContext.h	            \
	BufferAnalysis.h

AM_CPPFLAGS = -I@top_srcdir@/Events \
	-I@top_srcdir@/NSCLException \
	-I@top_srcdir@/Utility   \
	-I.  @LIBTCLPLUS_CFLAGS@

libBufferAnalysis_la_LDFLAGS = -version-info $(SOVERSION):0 @LIBTCLPLUS_LDFLAGS@


man_MANS = BufferAnalysis.3 CBufferProcessor.3 CBufferCallback.3


EXTRA_DIST = $(man_MANS)
