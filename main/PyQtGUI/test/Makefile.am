noinst_PROGRAMS = python_import.test

python_import_test_SOURCES = ImportTest.cpp ImportTest.h TestRunner.cpp

python_import_test_CXXFLAGS = -std=c++11 @CPPUNIT_CFLAGS@ -I/usr/include/python3.7

python_import_test_LDADD = @CPPUNIT_LIBS@

python_import_test_LDFLAGS = -L/usr/local/lib/python3.7 -lpython3.7m -Wl,-rpath,/usr/local/lib/python3.7 

TESTS = python_import.test python_methods.test
