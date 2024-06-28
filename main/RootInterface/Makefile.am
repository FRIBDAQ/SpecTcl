lib_LTLIBRARIES=libRootInterface.la

libRootInterface_la_SOURCES=TreeBuilder.h TreeBuilder.cpp \
	RootTreeSink.h RootTreeSink.cpp \
	TreeCommand.h TreeCommand.cpp CRootExec.h CRootExec.cpp \
	CRootExitCommand.h CRootExitCommand.cpp \
	rootPackage.cpp RootEventLoop.h



libRootInterface_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
	-I@top_srcdir@/factories -I@top_srcdir@/mpi				\
	@ROOT_CFLAGS@ @LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@

libRootInterface_la_LIBADD=@top_builddir@/mpi/libMPIUtilities.la \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @ROOT_LDFLAGS@

include_HEADERS=TreeBuilder.h

install-exec-local:

clean-local:


noinst_PROGRAMS=unittests

unittests_SOURCES=TestRunner.cpp treefoldertests.cpp treeterminaltests.cpp \
	paramtreetests.cpp marshallertests.cpp

unittests_LDADD = libRootInterface_la-TreeBuilder.lo \
	@top_builddir@/mpi/libMPIUtilities.la \
	@top_builddir@/Core/libTclGrammerApp_la-Event.lo \
	@top_builddir@/Core/libTclGrammerApp_la-DopeVector.lo \
	@CPPUNIT_LIBS@ @ROOT_LDFLAGS@ @LIBEXCEPTION_LDFLAGS@ @TCL_LIBS@
unittests_CXXFLAGS=@CPPUNIT_CFLAGS@ $(ROOT_CFLAGS) -I@top_srcdir@/Core \
	-I@top_srcdir@/Utility -I@top_srcdir@/mpi

TESTS_ENVIRONMENT=LD_LIBRARY_PATH=$(ROOT_LIBRARY_DIR)
TESTS=unittests

