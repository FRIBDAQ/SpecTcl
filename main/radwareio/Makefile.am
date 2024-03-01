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



libradwareio_la_CPPFLAGS= -I. $(SPECTCLCCSW) \
				@TCL_CPPFLAGS@  @ROOT_CFLAGS@ @LIBTCLPLUS_CFLAGS@

libradwareio_la_LDFLAGS=@ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@



EXTRA_DIST = permission.txt radwareio.xml

install-exec-local: radwareio.pdf
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) @datarootdir@/radwareio-plugin
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done
	$(INSTALL_DATA) @builddir@/radwareio.pdf @datarootdir@/radwareio-plugin


radwareio.pdf: radwareio.xml
	@PDFDOCBOOK@ @srcdir@/radwareio.xml

docs: radwareio.pdf


