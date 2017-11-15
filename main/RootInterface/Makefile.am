lib_LTLIBRARIES=libRootInterface.la

libRootInterface_la_SOURCES=TreeBuilder.h TreeBuilder.cpp 

libRootInterface_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
	@ROOT_CFLAGS@

noinst_HEADERS=TreeBuilder.h

install-exec-local:

clean-local:


noinst_PROGRAMS=unittests

unittests_SOURCES=TestRunner.cpp treefoldertests.cpp treeterminaltests.cpp \
	paramtreetests.cpp

unittests_LDADD = TreeBuilder.lo @CPPUNIT_LIBS@
unittests_CXXFLAGS=@CPPUNIT_CFLAGS@

TESTS=unittests
