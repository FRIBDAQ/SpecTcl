
lib_LTLIBRARIES=libMirrorClient.la
bin_PROGRAMS=mirrorclient


RESTCLIENTROOT=/usr/opt/restclient



mirrorclient_SOURCES=cmdline.c cimdline.h mirror.cpp \
	CmdInfo.h CmdInfo.cpp \
	MirrorCommand.h MirrorCommand.cpp 



mirrorclient_CXXFLAGS=-DSPECTCL_TCLLIBS="\"@prefix@/TclLibs\"" \
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@  

mirrorclient_LDADD=@builddir@/libMirrorClient.la \
	-L$(RESTCLIENTROOT)/lib -lrestclient-cpp \
	@top_builddir@/Display/libXamine.la \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @JSONCPP_LIBS@

mirrorclient_LDFLAGS=	-Wl,-rpath=$(RESTCLIENTROOT)/lib 


BUILT_SOURCES = cmdline.c cmdline.h 
noinst_HEADERS=cmdline.h

cmdline.h cmdline.c : @srcdir@/mirroropts.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/mirroropts.ggo

libMirrorClient_la_INCLUDES=MirrorClient.h SpecTclMirrorClient.h
libMirrorClient_la_SOURCES=MirrorClient.cpp SpecTclMirrorClient.cpp \
	CPortManager.cpp CPortManager.h \
	CPortManagerException.h CPortManagerException.cpp \
	MirrorClientInternals.h MirrorClientInternals.cpp
libMirrorClient_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Display \
	-I$(RESTCLIENTROOT)/include @LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ \
	@JSONCPP_CFLAGS@
libMirrorClient_la_LIBADD=@top_builddir@/Core/libTclGrammerApp.la \
	-L$(RESTCLIENTROOT)/lib -lrestclient-cpp \
	@LIBEXCEPTION_LDFLAGS@ @TCL_LIBS@ @JSONCPP_LIBS@


libMirrorClient_la_LDFLAGS=-Wl,-rpath=$(RESTCLIENTROOT)/lib 

install-exec-local:
	$(INSTALL_SCRIPT) @srcdir@/mirrorclientscript.tcl @bindir@

#############################     TESTS ###################################

noinst_PROGRAMS=unittests
unittests_SOURCES=TestRunner.cpp mctests.cpp
unittests_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Display \
	@CPPUNIT_CFLAGS@ \
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ \
	-I@top_srcdir@/Utility @JSONCPP_CFLAGS@
unittests_LDFLAGS=@builddir@/libMirrorClient.la \
	@top_builddir@/Core/libTclGrammerApp.la \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @ROOT_LDFLAGS@ @CPPUNIT_LIBS@ \
	$(libMirrorClient_la_LIBADD) $(libMirrorClient_la_LDFLAGS)




TESTS=unittests

EXTRA_DIST=mirrorclientscript.tcl
