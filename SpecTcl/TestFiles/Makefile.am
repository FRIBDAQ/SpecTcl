INCLUDES= -I. -I../EventSource -I../TCL -I../NSCLException \
	-I../Utility @TCL_FLAGS@ -I../Tape

LDADD = -lEventSource -lException -ltape



bin_PROGRAMS = testfile ltwrite

testfile_SOURCES = testfile.cpp

testfile_LDFLAGS= -L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
	-L../Tape \
	-L../Filter -L../SpecTcl -Wl,"-rpath=$(libdir)"


ltwrite_SOURCES = ltwrite.cpp

ltwrite_LDFLAGS= -L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
	-L../Tape \
	-L../Filter -L../SpecTcl -Wl,"-rpath=$(libdir)"