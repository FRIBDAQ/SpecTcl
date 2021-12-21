bin_PROGRAMS=rootxamine

RESTCLIENTROOT=/usr/opt/restclient

rootxamine_SOURCES=RootXamine.cpp cmdline.h cmdline.c
rootxamine_CPPFLAGS=-I@top_srcdir@/mirrorclient @ROOT_CFLAGS@ \
	-I$(RESTCLIENTROOT)/include @JSONCPP_CFLAGS@
rootxamine_LDADD=@top_builddir@/mirrorcient/libMirrorClient.la @ROOT_LDFLAGS@ \
	-L$(RESTCLIENTROOT)/lib -lrestclient-cpp \
	@JSONCPP_LIBS@

BUILT_SOURCES=cmdline.h cmdline.c
noinst_HEADERS=cmdline.h

cmdline.h cmdline.c : @srcdir@/rxoptions.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/rxoptions.ggo

EXTRA_DIST=rxoptions.ggo
