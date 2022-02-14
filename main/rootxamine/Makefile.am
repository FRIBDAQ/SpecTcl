bin_PROGRAMS=rootxamine


rootxamine_SOURCES=RootXamine.cpp cmdline.h cmdline.c \
	HistogramManager.h HistogramManager.cpp
rootxamine_CPPFLAGS=-I@top_srcdir@/mirrorclient @ROOT_CFLAGS@ \
	@RESTCLIENT_CPPFLAGS@  @JSONCPP_CFLAGS@ \
	-I@top_srcdir@/Display

rootxamine_LDADD=@top_builddir@/mirrorclient/libMirrorClient.la \
	@top_builddir@/Display/libXamine.la \
	@ROOT_LDFLAGS@ \
	-L$(RESTCLIENTROOT)/lib -lrestclient-cpp \
	@JSONCPP_LIBS@ @RESTCLIENT_LDFLAGS@

rootxamine_LDFLAGS=-Wl,-rpath=$(RESTCLIENTROOT)/lib 

BUILT_SOURCES=cmdline.h cmdline.c @builddir@/docs_built


noinst_HEADERS=cmdline.h

install-exec-local:
	$(mkinstalldirs) @prefix@/share/rootxamine
	$(INSTALL_DATA) @builddir@/rootxamine_docs/* @prefix@/share/rootxamine


cmdline.h cmdline.c : @srcdir@/rxoptions.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/rxoptions.ggo


@builddir@/docs_built : @srcdir@/rootxamine.xml
	@HTMLDOCBOOK@ @srcdir@/rootxamine.xml -o @builddir@/rootxamine_docs
	touch @builddir@/docs_built

EXTRA_DIST=rxoptions.ggo rootxamine.xml
