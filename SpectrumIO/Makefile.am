DEFS = @WCHAR_T_DEF@
INCLUDES = -I.. -I. \
	-I../SpecTcl -I../Sorter -I../Utility \
	-I../Xamine -I../Events -I../Gates -I../Display @TCL_CPPFLAGS@ \
	@LIBTCLPLUS_CFLAGS@
LDADD =  @LIBTCLPLUS_LDFLAGS@ -lXt -lX11 -L. @TK_LIBS@ @TCL_LIBS@
# AM_LDFLAGS = -L. @TK_LIBS@ @TCL_LIBS@

lib_LTLIBRARIES = libSpecio.la

libSpecio_la_LDFLAGS = $(SOVERSION):0 -Wl,"-rpath=$(libdir)"

libSpecio_la_SOURCES = SpectrumFormatter.cpp \
	SpectrumFormatterFactory.cpp \
	SpectrumStandardFormatters.cpp \
	NSCLBinarySpectrumFormatter.cpp \
	NSCLAsciiSpectrumFormatter.cpp  \
	SpectrumFormatError.cpp \
	nsclbin.cpp nsclbinerror.cpp nscldata.cpp tclstreams.cpp

include_HEADERS = NSCLAsciiSpectrumFormatter.h NSCLBinarySpectrumFormatter.h \
	SpectrumFormatError.h SpectrumFormatter.h SpectrumFormatterFactory.h \
	nsclbin.h nsclbinerror.h nscldata.h nscltime.h tclstreams.h

EXTRA_DIST = ClassDiagram.WC ClassDiagram.omt
