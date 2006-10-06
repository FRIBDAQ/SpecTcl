
INCLUDES = 	-I@top_srcdir@/Utility	\
	  	-I@top_srcdir@/Xamine	\
		-I@top_srcdir@/Exception \
		-I@top_srcdir@/Sorter    \
		-I@top_srcdir@/SpecTcl	\
		-I@top_srcdir@/Events	\
		-I@top_srcdir@/Display	\
		-I@top_srcdir@/Gates	\
		-I@top_srcdir@/Filter	\
		-I@top_srcdir@/TCL	\
		$(TCL_FLAGS)


lib_LTLIBRARIES=libFits.la
libFits_la_LDFLAGS = -version-info $(SOVERSION):0  \
	-Wl,"-rpath-link=$(libdir)"

libFits_la_SOURCES = 	CFit.cpp		\
			CFitCreator.cpp		\
			CFitFactory.cpp		\
			CLinearFit.cpp		\
			CLinearFitCreator.cpp	\
			CSpectrumFit.cpp

include_HEADERS	=	CFit.h			\
			CFitCreator.h		\
			CFitFactory.h		\
			CLinearFit.h		\
			CLinearFitCreator.h	\
			CSpectrumFit.h

