lib_LTLIBRARIES = libCPyConverter.la 

libCPyConverter_la_SOURCES = CPyConverter.cpp dataRetriever.cpp dataAccess.cpp \
	CPyConverter.h dataRetriever.h dataAccess.h dataTypes.h

libCPyConverter_la_CXXFLAGS = -std=c++11 -c -g -fPIC @PYTHON_CXXFLAGS@ -DINSTALLED_IN='"@prefix@"' -I@top_srcdir@/mirrorclient -I@top_srcdir@/Core

libCPyConverter_la_LDFLAGS = -shared @PYTHON_LDFLAGS@ -version-info 0:0:0 \
	@top_builddir@/mirrorclient/libMirrorClient.la

install-exec-hook: @builddir@/libCPyConverter.la
	$(mkinstalldirs) @prefix@/QtGUI
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:@libdir@; export LD_LIBRARY_PATH;
