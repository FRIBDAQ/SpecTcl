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


INCLUDES		= -I. $(SPECTCLCCSW) \
				$(TCLCCSW) 


EXTRA_DIST = permission.txt

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done

