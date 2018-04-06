lib_LTLIBRARIES=libfirstof.la

ROOT_CFLAGS=$(shell $(ROOTSYS)/bin/root-config --cflags)
ROOT_LDFLAGS=$(shell $(ROOTSYS)/bin/root-config --ldflags)

libfirstof_la_SOURCES 	=	CFirstofCommand.cpp		\
				CFirstofEventProcessor.cpp	\
				firstofPackage.cpp 		\
				CBiggestOfEventProcessor.cpp
noinst_HEADERS	=	CFirstofCommand.h 	\
			CFirstofEventProcessor.h \
			CBiggestOfEventProcessor.h

AM_CPPFLAGS=$(SPECTCLCCSW) $(TCLCCSW) $(ROOT_CFLAGS)

libfirsto_la_LDFLAGS = $(ROOT_LDFLAGS)


libfirstof_la_LDFLAGS=$(ROOT_LDFLAGS)

libfirstof_la_LDFLAGS=$(ROOT_LDFLAGS)

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done



EXTRA_DIST	=	firstof.xml

firstof.dvi: firstof.xml
	@DOCBOOK@ firstof.xml

firstof.pdf: firstof.dvi
	@DVIPDF@ firstof.dvi

docs: firstof.pdf

