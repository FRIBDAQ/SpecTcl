#lib_LTLIBRARIES = libBufferAnalysis.la
#
#libBufferAnalysis_la_SOURCES = CBufferCallbackList.cpp \
#	CBufferProcessor.cpp         \
#	CLanguageCallback.cpp        \
#	CStandardControlCallback.cpp \
#	CStandardScalerCallback.cpp  \
#	BufferAnalysis.cpp		\
#	CAnalysisBase.cpp CAnalysisEventProcessor.cpp
#
#include_HEADERS = CBufferCallback.h \
#	CBufferCallbackList.h       \
#	CBufferProcessor.h          \
#	CLanguageCallback.h         \
#	CStandardControlCallback.h  \
#	CStandardScalerCallback.h   \
#	SRunContext.h	            \
#	BufferAnalysis.h	\
#	CAnalysisBase.h CAnalysisEventProcessor.h 
#
#libBufferAnalysis_la_CPPFLAGS = -I@top_srcdir@/Events \
#	-I@top_srcdir@/NSCLException \
#	-I@top_srcdir@/Utility   \
#	-I@top_srcdir@/SpecTcl   \
#	-I@top_srcdir@/DecoderRing \
#	-I@top_srcdir@/Filter	\
#	-I.  @LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@
#
#libBufferAnalysis_la_LDFLAGS = -version-info $(SOVERSION):0 @LIBTCLPLUS_LDFLAGS@


install-exec-local: docs_built
	$(mkinstalldirs) @prefix@/share @prefix@/share/analysis
	$(mkinstalldirs) @prefix@/share/images
	$(INSTALL_DATA) @builddir@/*.html @prefix@/share/analysis
	cat @srcdir@/images.tar.gz | (cd @prefix@/share/images; tar xzf -)


man_MANS = BufferAnalysis.3 CBufferProcessor.3 CBufferCallback.3




EXTRA_DIST = $(man_MANS) analysis.xml images.tar.gz

docs_built: analysis.xml
	@HTMLDOCBOOK@ $<
	touch docs_built


