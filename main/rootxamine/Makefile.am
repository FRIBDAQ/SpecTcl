bin_PROGRAMS=rootxamine

RESTCLIENTROOT=/usr/opt/restclient

rootxamine_SOURCES=RootXamine.cpp cmdline.h cmdline.c \
	HistogramManager.h HistogramManager.cpp
rootxamine_CPPFLAGS=-I@top_srcdir@/mirrorclient @ROOT_CFLAGS@ \
	-I$(RESTCLIENTROOT)/include @JSONCPP_CFLAGS@ \
	-I@top_srcdir@/Display

rootxamine_LDADD=@top_builddir@/mirrorclient/libMirrorClient.la \
	@top_builddir@/Display/libXamine.la \
	@ROOT_LDFLAGS@ \
	-L$(RESTCLIENTROOT)/lib -lrestclient-cpp \
	@JSONCPP_LIBS@

rootxamine_LDFLAGS=-Wl,-rpath=$(RESTCLIENTROOT)/lib 

BUILT_SOURCES=cmdline.h cmdline.c
noinst_HEADERS=cmdline.h

cmdline.h cmdline.c : @srcdir@/rxoptions.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/rxoptions.ggo

EXTRA_DIST=rxoptions.ggo
