LDFLAGS=@TCL_LDFLAGS@

INCLUDES = -I. -I@top_srcdir@/Utility -I@top_srcdir@/NSCLException \
	-I@top_srcdir@/Xamine -I@top_srcdir@/TCL \
	@TCL_FLAGS@ \
	-D_DEBUG -DCHECK_ALL -DDESIGN_BY_CONTRACT


lib_LTLIBRARIES=libCalibrations.la

libCalibrations_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libCalibrations_la_SOURCES = CFit.cpp  CFitCommand.cpp  CFitCreator.cpp  \
	CFitFactory.cpp  CLinearFit.cpp  CLinearFitCreator.cpp

include_HEADERS = CFit.h  CFitCommand.h  CFitCreator.h  CFitFactory.h  \
	CLinearFit.h  CLinearFitCreator.h
