INCLUDES= -I. -I../EventSource -I../TCL -I../NSCLException \
	-I../Utility @TCL_FLAGS@ -I../Tape

LDADD = -lFilter -lAnalysis   -lEventSource -ltclPlus -lException \
	-ltape -lTclGrammerCommands

LDFLAGS = -L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
	-L../Tape \
	-L../Filter -L../SpecTcl -Wl,"-rpath=$(libdir)"


bin_PROGRAMS = testfile ltwrite

testfile_SOURCES = testfile.cpp



ltwrite_SOURCES = ltwrite.cpp

