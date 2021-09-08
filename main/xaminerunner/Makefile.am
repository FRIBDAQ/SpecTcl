

#  Build the command line parser

BUILT_SOURCES =  cmdline.c cmdline.h
noinst_HEADERS=cmdline.h

cmdline.c cmdline.h: @srcdir@/xamineRunnerOptions.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/xamineRunnerOptions.ggo

# The xaminerunne program.

bin_PROGRAMS=xaminerunner

xaminerunner_SOURCES=cmdline.c xaminerunner.cpp cmdline.h
xaminerunner_CXXFLAGS=@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@
xaminerunner_LDADD=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @ROOT_LDFLAGS@

EXTRA_DIST = xamineRunnerOptions.goo
