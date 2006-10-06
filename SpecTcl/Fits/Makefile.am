
INCLUDES = 	-I@top_srcdir@/Utility	\
	  	-I@top_srcdir@/Xamine

lib_LTLIBRARIES=libFits.la
libFits_la_LDFLAGS = -version-info $(SOVERSION):0  \
	-Wl,"-rpath-link=$(libdir)"

libFits_la_SOURCES = 	CFit.cpp	\
			CFitCreator.cpp	\
			CFitFactory.cpp	\
			CLinearFit.cpp	\
			CLinearFitCreator.cpp

include_HEADERS	=	CFit.h			\
			CFitCreator.h		\
			CFitFactory.h		\
			CLinearFit.h		\
			CLinearFitCreator.h

