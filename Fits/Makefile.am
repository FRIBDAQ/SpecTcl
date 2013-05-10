
#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321
#


INCLUDES = 	-I@top_srcdir@/Utility	\
	  	-I@top_srcdir@/Xamine	\
		-I@top_srcdir@/NSCLException \
		-I@top_srcdir@/Sorter    \
		-I@top_srcdir@/SpecTcl	\
		-I@top_srcdir@/Events	\
		-I@top_srcdir@/Display	\
		-I@top_srcdir@/Gates	\
		-I@top_srcdir@/Filter	\
		-I@top_srcdir@/TCL	\
		-I@top_srcdir@/factories \
		@TCL_CPPFLAGS@ @TK_CPPFLAGS@


lib_LTLIBRARIES=libFits.la
libFits_la_LDFLAGS = -version-info $(SOVERSION):0  \
	-Wl,"-rpath-link=$(libdir)" @TK_LIBS@ @TCL_LIBS@ \
	-lgsl -lgslcblas -lm

libFits_la_SOURCES = 	CFit.cpp		\
			CFitCreator.cpp		\
			CFitFactory.cpp		\
			CLinearFit.cpp		\
			CLinearFitCreator.cpp	\
			CSpectrumFit.cpp	\
			CFitDictionary.cpp	\
			fitgsl.c		\
			CGaussianFit.cpp	\
			CGaussianFitCreator.cpp

include_HEADERS	=	CFit.h			\
			CFitCreator.h		\
			CFitFactory.h		\
			CLinearFit.h		\
			CLinearFitCreator.h	\
			CSpectrumFit.h		\
			CFitDictionary.h	\
			fitgsl.h		\
			CGaussianFit.h		\
			CGaussianFitCreator.h

EXTRA_DIST=permission.txt
