lib_LTLIBRARIES = libradwareio.la

libradwareio_la_SOURCES		= 	cask.c 			\
					caskyn.c		\
					matinv.c 		\
					matread.c		\
					readsp.c		\
					rwspec.c		\
					util.c			\
					radwareioPackage.cpp	\
					CRWWrite.cpp CRWRead.cpp

noinst_HEADERS			= 	util.h CRWWrite.h CRWRead.h


libradwareio_la_LIBADD  	= -lpub


INCLUDES		= -I. -I@top_builddir@ 		\
				$(TCL_FLAGS) -I@top_builddir@/TCL \
				-I@top_builddir@/Utility 	\
				-I@top_builddir@/SpecTcl	\
				-I@top_builddir@/Events		\
				-I@top_builddir@/Xamine 	\
				-I@top_builddir@/Sorter		\
				-I@top_builddir@/NSCLException	\
				-I@top_builddir@/Display        \
				-I@top_builddir@/Gates		\
				-I@top_builddir@/Filter
