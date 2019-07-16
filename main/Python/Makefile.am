lib_LTLIBRARIES=libSpecTclPython.la

libSpecTclPython_la_SOURCES=CPythonCommand.cpp CPythonCommand.h \
	PythonPackage.cpp


libSpecTclPython_la_CXXFLAGS=-I@top_srcdir@/Core \
	-I@PYTHON_INCLUDE_DIR@				\
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ 

libSpecTclPython_la_LIBADD= @top_builddir@/Core/libTclGrammerApp.la   \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ \
	-l@PYTHON_LIB@
