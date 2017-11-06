lib_LTLIBRARIES=libRootInterface.la

libRootInterface_la_SOURCES=TreeBuilder.h TreeBuilder.cpp



noinst_PROGRAMS=unittests

unittests_SOURCES=TestRunner.cpp treefoldertests.cpp

unittests_LDADD = TreeBuilder.lo @CPPUNIT_LIBS@
unittests_CXXFLAGS=@CPPUNIT_CFLAGS@

TESTS=unittests
