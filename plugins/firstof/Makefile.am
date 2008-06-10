lib_LTLIBRARIES=libfirstof.la
libfirstof_la_SOURCES 	=	CFirstofCommand.cpp		\
				CFirstofEventProcessor.cpp	\
				firstofPackage.cpp 		\
				CBiggestOfEventProcessor.cpp
noinst_HEADERS	=	CFirstofCommand.h 	\
			CFirstofEventProcessor.h \
			CBiggestOfEventProcessor.h

INCLUDES=$(SPECTCLCCSW) $(TCLCCSW)


install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done



EXTRA_DIST	=	firstof.xml

firstof.dvi: firstof.xml
	@DOCBOOK@ firstof.xml

firstof.pdf: firstof.dvi
	@DVIPDF@ firstof.dvi

docs: firstof.pdf

