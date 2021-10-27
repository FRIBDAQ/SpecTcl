# Defs:

SPECTCL_TCLLIBS=@prefix@/TclLibs
PACKAGE_DESTDIR = $(SPECTCL_TCLLIBS)/Xamine


#  Build the command line parser

BUILT_SOURCES =  cmdline.c cmdline.h
noinst_HEADERS=cmdline.h

cmdline.c cmdline.h: @srcdir@/xamineRunnerOptions.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/xamineRunnerOptions.ggo

# TCL Packages:

TCLPACKAGE_FILES=XamineUtils.tcl

install-exec-local:
	$(mkinstalldirs) $(PACKAGE_DESTDIR)
	for f in $(TCLPACKAGE_FILES);   \
	do \
		$(INSTALL_SCRIPT) @srcdir@/$$f $(PACKAGE_DESTDIR); \
	done
	echo pkg_mkIndex -verbose $(PACKAGE_DESTDIR) "*.tcl" | $(TCLSH)
	$(INSTALL_SCRIPT) @srcdir@/xaminerunner.tcl @bindir@
	$(HTMLDOCBOOK) -o @datarootdir@/saXamine @srcdir@/saloneXamine.xml

# The xaminerunne program.


bin_PROGRAMS=xaminerunner

xaminerunner_SOURCES=cmdline.c xaminerunner.cpp cmdline.h \
	Info.h Info.cpp   \
	TclXamine.h TclXamine.cpp \
	XamineGateHandlers.h XamineGateHandlers.cpp Mirror.h Mirror.cpp

xaminerunner_CXXFLAGS=@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ \
	-DSPECTCL_TCLLIBS="\"$(SPECTCL_TCLLIBS)\"" -I@top_srcdir@/Display \
	-I@top_srcdir@/Utility -I@top_srcdir@/mirrorclient
xaminerunner_LDADD=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @ROOT_LDFLAGS@ \
	@top_builddir@/Utility/libUtilities.la \
	@top_builddir@/Display/libXamine.la \
	@top_builddir@/mirrorclient/libMirrorClient.la

xaminerunner_LDFAGS=-Wl,-rpath=@prefix@/lib

EXTRA_DIST = xamineRunnerOptions.ggo $(TCLPACKAGE_FILES) xaminerunner.tcl \
	saloneXamine.xml XamineUtils.tcl 
