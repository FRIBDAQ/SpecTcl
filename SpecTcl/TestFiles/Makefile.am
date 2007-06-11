INCLUDES= -I. -I../EventSource -I../TCL -I../NSCLException \
	-I../Utility @TCL_FLAGS@ -I../Tape

bin_PROGRAMS = testfile ltwrite

testfile_LDFLAGS= -Wl,"-rpath=$(libdir)"

testfile_LDADD= -L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
	-L../Tape \
	-L../Filter -L../SpecTcl -lEventSource -lException -ltape

ltwrite_LDFLAGS= -Wl,"-rpath=$(libdir)"
ltwrite_LDADD= -L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
	-L../Tape \
	-L../Filter -L../SpecTcl -lEventSource -lException -ltape


testfile_SOURCES = testfile.cpp



ltwrite_SOURCES = ltwrite.cpp

