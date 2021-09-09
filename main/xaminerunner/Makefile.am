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

# The xaminerunne program.


bin_PROGRAMS=xaminerunner

xaminerunner_SOURCES=cmdline.c xaminerunner.cpp cmdline.h \
	Info.h Info.cpp   \
	TclXamine.h TclXamine.cpp
xaminerunner_CXXFLAGS=@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ \
	-DSPECTCL_TCLLIBS="\"$(SPECTCL_TCLLIBS)\"" -I@top_srcdir@/Display
xaminerunner_LDADD=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @ROOT_LDFLAGS@ \
	@top_builddir@/Display/libXamine.la

xaminerunner_LDFAGS=-Wl,-rpath=@prefix@/lib

EXTRA_DIST = xamineRunnerOptions.ggo $(TCLPACKAGE_FILES)
