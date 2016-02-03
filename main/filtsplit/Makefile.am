SUFFIXES = .xml .1

 .xml.1:
	-$(MANDOCBOOK) man $< 2>/dev/null
	touch $@

bin_PROGRAMS=filtsplit

filtsplit_SOURCES=cmdline.c filtsplit.cpp

noinst_HEADERS=cmdline.h

filtsplit_CPPFLAGS =-I@top_srcdir@/Core


cmdline.c cmdline.h: filtsplit.ggo
	@GENGETOPT@ --unamed-opts <filtsplit.ggo

EXTRA_DIST=filtsplit.ggo  filtsplit.xml


filtsplit_LDADD = @top_builddir@/Core/libSpecTcl.la \
		  @LIBTCLPLUS_LDFLAGS@ \
		  @TCL_LIBS@ \
		  -lgsl -lgslcblas


man_MANS = filtsplit.1
