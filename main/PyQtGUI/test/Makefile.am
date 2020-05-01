noinst_PROGRAMS = python_import.test

python_import_test_SOURCES = ImportTest.cpp ImportTest.h TestRunner.cpp

python_import_test_CXXFLAGS = -std=c++11 @CPPUNIT_CFLAGS@ @PYTHON_CXXFLAGS@

python_import_test_LDADD = @CPPUNIT_LIBS@

python_import_test_LDFLAGS = @PYTHON_LDFLAGS@

AM_TESTS_ENVIRONMENT=TESTSRC=@srcdir@; export TESTSRC;
TESTS = python_import.test python_methods.test

EXTRA_DIST = python_methods.py python_methods.test
