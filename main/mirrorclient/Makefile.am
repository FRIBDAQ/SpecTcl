
lib_LTLIBRARIES=libMirrorClient.la
bin_PROGRAMS=mirrorclient




mirrorclient_SOURCES=cmdline.c cimdline.h mirror.cpp \
	CmdInfo.h CmdInfo.cpp \
	MirrorCommand.h MirrorCommand.cpp

mirrorclient_CXXFLAGS=-DSPECTCL_TCLLIBS="\"@prefix@/TclLibs\"" \
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@
mirrorclient_LDADD=@builddir@/libMirrorClient.la \
	@top_builddir@/Display/libXamine.la \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@

BUILT_SOURCES = cmdline.c cmdline.h 
noinst_HEADERS=cmdline.h

cmdline.h cmdline.c : @srcdir@/mirroropts.ggo
	@GENGETOPT@ --unamed-opts <@srcdir@/mirroropts.ggo

libMirrorClient_la_INCLUDES=MirrorClient.h
libMirrorClient_la_SOURCES=MirrorClient.cpp
libMirrorClient_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Display
libMirrorClient_la_LIBADD=@top_builddir@/Core/libTclGrammerApp.la

install-exec-local:
	$(INSTALL_SCRIPT) @srcdir@/mirrorclientscript.tcl @bindir@

#############################     TESTS ###################################

noinst_PROGRAMS=unittests
unittests_SOURCES=TestRunner.cpp mctests.cpp
unittests_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Display @CPPUNIT_CFLAGS@ \
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ -I@top_srcdir@/Utility
unittests_LDFLAGS=@builddir@/libMirrorClient.la \
	@top_builddir@/Core/libTclGrammerApp.la \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @ROOT_LDFLAGS@ @CPPUNIT_LIBS@



TESTS=unittests

EXTRA_DIST=mirrorclientscript.tcl
