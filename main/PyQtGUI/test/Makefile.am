noinst_PROGRAMS = python_import.test

python_import_test_SOURCES = ImportTest.cpp ImportTest.h TestRunner.cpp

python_import_test_CXXFLAGS = -std=c++11 @CPPUNIT_CFLAGS@ @PYTHON_CXXFLAGS@

python_import_test_LDADD = @CPPUNIT_LIBS@

python_import_test_LDFLAGS = @PYTHON_LDFLAGS@

TESTS = python_import.test python_methods.test
