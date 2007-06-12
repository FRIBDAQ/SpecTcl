INCLUDES= -I. -I../EventSource -I../TCL -I../NSCLException \
	-I../Utility @TCL_FLAGS@ -I../Tape

LDADD = -lEventSource -lException -ltape


bin_PROGRAMS = testfile ltwrite

testfile_LDFLAGS=-L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
	-L../Tape \
	-L../Filter -L../SpecTcl -Wl,"-rpath=$(libdir)"


ltwrite_LDFLAGS= -L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
	-L../Tape \
	-L../Filter -L../SpecTcl -Wl,"-rpath=$(libdir)"


testfile_SOURCES = testfile.cpp



ltwrite_SOURCES = ltwrite.cpp

