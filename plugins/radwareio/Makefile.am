lib_LTLIBRARIES = libradwareio.la
ROOT_CFLAGS=$(shell $(ROOTSYS)/bin/root-config --cflags)
ROOT_LDFLAGS=$(shell $(ROOTSYS)/bin/root-config --ldflags)

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



AM_CPPFLAGS		= -I. $(SPECTCLCCSW) \
				$(TCLCCSW) $(ROOT_CFLAGS)

libradwareio_la_LDFLAGS = $(ROOT_LDFLAGS)


EXTRA_DIST = permission.txt radwareio.xml

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done


radwareio.pdf: radwareio.xml
	@DOCBOOK@ radwareio.xml

docs: radwareio.pdf


