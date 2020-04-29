lib_LTLIBRARIES = libCPyConverter.la 

libCPyConverter_la_SOURCES = CPyConverter.cpp dataRetriever.cpp dataAccess.cpp \
	CPyConverter.h dataRetriever.h dataAccess.h dataTypes.h

libCPyConverter_la_CXXFLAGS = -std=c++11 -c -g -fPIC -I/usr/include/python3.7 

libCPyConverter_la_LDFLAGS = -shared -L/usr/local/lib/python3.7 -lpython3.7m -Wl,-rpath,/usr/local/lib/python3.7 -version-info 0:0:0

install-exec-hook: @builddir@/libCPyConverter.la
	$(mkinstalldirs) @prefix@/QtGUI
	rm -f @prefix@/QtGUI/libCPyConverter.so
	rm -f @prefix@/QtGUI/libCPyConverter.so.0
	rm -f @prefix@/QtGUI/libCPyConverter.so.0.0.0
	$(LN_S)           @libdir@/libCPyConverter.so   @prefix@/QtGUI/libCPyConverter.so
	$(LN_S)           @libdir@/libCPyConverter.so.0   @prefix@/QtGUI/libCPyConverter.so.0
	$(LN_S)           @libdir@/libCPyConverter.so.0.0.0   @prefix@/QtGUI/libCPyConverter.so.0.0.0

