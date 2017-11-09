lib_LTLIBRARIES=libRootInterface.la

libRootInterface_la_SOURCES=TreeBuilder.h TreeBuilder.cpp CEventLeaf.cpp 

libRootInterface_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
	@ROOT_CFLAGS@

include_HEADERS=CEventLeaf.h

nodist_libRootInterface_la_SOURCES=CEventLeafDict.C
BUILT_SOURCES= CEventLeafDict.C

CEventLeafDict.C: CEventLeaf.h
	`dirname @ROOTCONFIG@`/rootcint -f CEventLeafDict.C -c -p  CEventLeaf.h

install-exec-local:
	$(INSTALL_DATA) @builddir@/CEventLeafDict_rdict.pcm @libdir@

noinst_PROGRAMS=unittests

unittests_SOURCES=TestRunner.cpp treefoldertests.cpp treeterminaltests.cpp \
	paramtreetests.cpp

unittests_LDADD = TreeBuilder.lo @CPPUNIT_LIBS@
unittests_CXXFLAGS=@CPPUNIT_CFLAGS@

TESTS=unittests
