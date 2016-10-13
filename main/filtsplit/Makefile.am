SUFFIXES = .xml .1

 .xml.1:
	-$(MANDOCBOOK) man $< 2>/dev/null
	touch $@

bin_PROGRAMS=filtsplit

BUILT_SOURCES = cmdline.h cmdline.c
filtsplit_SOURCES = cmdline.c filtsplit.cpp

noinst_HEADERS=cmdline.h

filtsplit_CPPFLAGS =-I@top_srcdir@/Core
filtsplit_CFLAGS =-I@top_srcdir@/Core
filtsplit_CXXFLAGS =-I@top_srcdir@/Core

filtsplit_LDADD = @top_builddir@/Core/libTclGrammerApp.la \
		  @LIBTCLPLUS_LDFLAGS@ \
		  @TCL_LIBS@ \
		  -lgsl -lgslcblas

cmdline.c cmdline.h: @srcdir@/filtsplit.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/filtsplit.ggo

EXTRA_DIST=filtsplit.ggo  filtsplit.xml




man_MANS = filtsplit.1
