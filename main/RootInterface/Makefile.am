lib_LTLIBRARIES=libRootInterface.la

libRootInterface_la_SOURCES=TreeBuilder.h TreeBuilder.cpp 

libRootInterface_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
	$(ROOT_CFLAGS)

include_HEADERS=TreeBuilder.h

install-exec-local:

clean-local:


noinst_PROGRAMS=unittests

unittests_SOURCES=TestRunner.cpp treefoldertests.cpp treeterminaltests.cpp \
	paramtreetests.cpp marshallertests.cpp

unittests_LDADD = libRootInterface_la-TreeBuilder.lo \
	@top_builddir@/Core/libTclGrammerApp_la-Event.lo \
	@top_builddir@/Core/libTclGrammerApp_la-DopeVector.lo \
	@CPPUNIT_LIBS@ @ROOT_LDFLAGS@ @LIBEXCEPTION_LDFLAGS@
unittests_CXXFLAGS=@CPPUNIT_CFLAGS@ $(ROOT_CFLAGS) -I@top_srcdir@/Core \
	-I@top_srcdir@/Utility


TESTS=unittests
