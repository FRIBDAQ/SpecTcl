SUFFIXES = .xml .1

 .xml.1:
	-$(MANDOCBOOK) man $< 2>/dev/null
	touch $@

bin_PROGRAMS=filtsplit

filtsplit_SOURCES=cmdline.c filtsplit.cpp

noinst_HEADERS=cmdline.h

INCLUDES=-I@top_srcdir@/Filter


cmdline.c: filtsplit.ggo
	gengetopt --unamed-opts <filtsplit.ggo

EXTRA_DIST=filtsplit.ggo  filtsplit.xml


filtsplit_LDADD = @top_srcdir@/Filter/CXdrInputStream.lo \
		@top_srcdir@/Filter/CXdrOutputStream.lo

man_MANS = filtsplit.1