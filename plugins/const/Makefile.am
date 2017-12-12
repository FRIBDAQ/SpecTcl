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
				$(TCLCCSW) $(ROOT_CFLAGS)

libconstparam_la_LDFLAGS=$(ROOT_LDFLAGS)

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs $(prefix)/TclLibs/constparam
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs/constparam; done
	echo package ifneeded constparam 1.0 [list load [file join '$$dir' libconstparam.so]] > $(prefix)/TclLibs/constparam/pkgIndex.tcl

EXTRA_DIST	= const.xml

const.pdf:	const.xml
	@DOCBOOK@ const.xml


docs:   const.pdf
