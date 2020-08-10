lib_LTLIBRARIES=libSpecTclPython.la

libSpecTclPython_la_SOURCES=CPythonCommand.cpp CPythonCommand.h \
	PythonPackage.cpp \
	pythonParameters.cpp pythonParameters.h \
	pythonSpectrum.cpp pythonSpectrum.h 	\
	pythonVariable.cpp pythonVariable.h	\
	pythonGates.cpp pythonGates.h


libSpecTclPython_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
	@PYTHON_CXXFLAGS@ -I@top_srcdir@/factories -I@top_srcdir@/ddas \
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ -Wno-write-strings

libSpecTclPython_la_LIBADD= @top_builddir@/Core/libTclGrammerApp.la   \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ \
	@PYTHON_LDFLAGS@

install-exec-hook: @builddir@/libSpecTclPython.la
	$(mkinstalldirs) @prefix@/TclLibs/python
	$(INSTALL_PROGRAM) @libdir@/libSpecTclPython.so @prefix@/TclLibs/python
	echo package ifneeded python 1.0 [list load [file join \$$dir libSpecTclPython.so]] > @prefix@/TclLibs/python/pkgIndex.tcl
	@HTMLDOCBOOK@ -o @prefix@/share/python @srcdir@/python.xml


EXTRA_DIST=python.xml

