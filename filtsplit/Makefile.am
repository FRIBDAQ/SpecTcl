bin_PROGRAMS=filtsplit

filtsplit_SOURCES=filtsplit.cpp cmdline.c

noinst_HEADERS=cmdline.h

INCLUDES=-I@top_srcdir@/Filter


cmdline.c: filtsplit.ggo
	gengetopt --unamed-opts <filtsplit.ggo

EXTRA_DIST=daqstart.ggo


filtsplit_LDADD = @top_srcdir@/Filter/CXdrInputStream.lo \
		@top_srcdir@/Filter/CXdrOutputStream.lo