lib_LTLIBRARIES=libfirstof.la

ROOTCFLAGS=$(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLDFLAGS=$(shell $(ROOTSYS)/bin/root-config --ldflags)

libfirstof_la_SOURCES 	=	CFirstofCommand.cpp		\
				CFirstofEventProcessor.cpp	\
				firstofPackage.cpp 		\
				CBiggestOfEventProcessor.cpp
noinst_HEADERS	=	CFirstofCommand.h 	\
			CFirstofEventProcessor.h \
			CBiggestOfEventProcessor.h

AM_CPPFLAGS=$(SPECTCLCCSW) $(TCLCCSW) $(ROOT_CFLAGS)

libfirstof_la_LDFLAGS = $(ROOTLDFLAGS)
libfirstof_la_CPPFLAGS=$(ROOTCFLAGS)

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done



EXTRA_DIST	=	firstof.xml

firstof.dvi: firstof.xml
	@DOCBOOK@ firstof.xml

firstof.pdf: firstof.dvi
	@DVIPDF@ firstof.dvi

docs: firstof.pdf

