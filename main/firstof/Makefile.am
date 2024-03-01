lib_LTLIBRARIES=libfirstof.la


libfirstof_la_SOURCES 	=	CFirstofCommand.cpp		\
				CFirstofEventProcessor.cpp	\
				firstofPackage.cpp 		\
				CBiggestOfEventProcessor.cpp
noinst_HEADERS	=	CFirstofCommand.h 	\
			CFirstofEventProcessor.h \
			CBiggestOfEventProcessor.h


libfirstof_la_LDFLAGS = @ROOT_LDFLAGS@ @TCL_LIBS@ @LIBTCLPLUS_LDFLAGS@
libfirstof_la_CPPFLAGS=@ROOT_CFLAGS@ @TCL_CPPFLAGS@ @LIBTCLPLUS_CFLAGS@ \
	-I@top_srcdir@/Core -I@top_srcdir@/Utility -I@top_srcdir@/factories

install-exec-local: firstof.pdf
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) @prefix@/share/firstof-plugin
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done
	$(INSTALL_DATA) @builddir@/firstof.pdf @prefix@/share/firstof-plugin



EXTRA_DIST	=	firstof.xml


firstof.pdf: firstof.xml
	@PDFDOCBOOK@ @srcdir@/firstof.xml

docs: firstof.pdf

