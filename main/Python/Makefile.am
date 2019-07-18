lib_LTLIBRARIES=libSpecTclPython.la

libSpecTclPython_la_SOURCES=CPythonCommand.cpp CPythonCommand.h \
	PythonPackage.cpp \
	pythonParameters.cpp pythonParameters.h


libSpecTclPython_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
	-I@PYTHON_INCLUDE_DIR@ -I@top_srcdir@/factories		\
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ 

libSpecTclPython_la_LIBADD= @top_builddir@/Core/libTclGrammerApp.la   \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ \
	-l@PYTHON_LIB@

install-exec-hook: @builddir@/libSpecTclPython.la
	$(mkinstalldirs) @prefix@/TclLibs/python
	$(INSTALL_PROGRAM) @libdir@/libSpecTclPython.so @prefix@/TclLibs/python
	echo package ifneeded python 1.0 [list load [file join \$$dir libSpecTclPython.so]] > @prefix@/TclLibs/python/pkgIndex.tcl

