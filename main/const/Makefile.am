lib_LTLIBRARIES  = libconstparam.la

ROOT_CFLAGS=$(shell $(ROOTSYS)/bin/root-config --cflags)
ROOT_LDFLAGS=$(shell $(ROOTSYS)/bin/root-config --ldflags)

libconstparam_la_SOURCES = 	CConstCommand.cpp  \
				CConstData.cpp	\
				CConstPackage.cpp \
				 CConstProcessor.cpp

noinst_HEADERS = 	CConstCommand.h  \
			CConstData.h  \
			CConstProcessor.h

AM_CPPFLAGS		= -I. $(SPECTCLCCSW) \
				@TCL_CPPFLAGS@ @ROOT_CFLAGS@ @LIBTCLPLUS_CFLAGS@

libconstparam_la_LDFLAGS=@ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@

install-exec-local: const.pdf
	$(mkinstalldirs) @prefix@/share/const-plugin
	$(mkinstalldirs) $(prefix)/TclLibs $(prefix)/TclLibs/constparam
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs/constparam; done
	echo package ifneeded constparam 1.0 [list load [file join '$$dir' libconstparam.so]] > $(prefix)/TclLibs/constparam/pkgIndex.tcl
	$(INSTALL_DATA) @builddir@/const.pdf @prefix@/share/const-plugin

EXTRA_DIST	= const.xml

const.pdf:	const.xml
	@PDFDOCBOOK@ @srcdir@/const.xml


docs:   const.pdf
