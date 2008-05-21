lib_LTLIBRARIES=libfirstof.la
libfirstof_la_SOURCES 	=	CFirstofCommand.cpp		\
				CFirstofEventProcessor.cpp	\
				firstofPackage.cpp 		
noinst_HEADERS	=	CFirstofCommand.h 	\
			CFirstofEventProcessor.h

INCLUDES=$(SPECTCLCCSW) $(TCLCCSW)


install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done



