lib_LTLIBRARIES=libSpecTclPython.la

libSpecTclPython_la_SOURCES=CPythonCommand.cpp CPythonCommand.h

libSpecTclPython_la_CXXFLAGS=-I@top_srcdir@/Core \
	-I@PYTHON_INCLUDE_DIR@				\
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ 

libSpecTclPython_la_LIBADD=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ \
	-l@PYTHON_LIB@
