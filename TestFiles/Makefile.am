# INCLUDES = -I. -I../Utility -I../EventSource -I../Tape -I../NSCLException
INCLUDES= -I. -I../EventSource -I../TCL -I../NSCLException -I../Utility @TCL_FLAGS@ -I../Tape
# LDADD = -lTclGrammerCommands -lXamine -lGates -lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException -lSpecio -lreplace -ltape -lm
LDADD = -lFilter -lAnalysis   -lEventSource -ltclPlus -lException -ltape -lTclGrammerCommands

LDFLAGS = -L. -L../Events -L../EventSource -L../TCL -L../NSCLException -L../Tape \
	-L../Filter 	-L../SpecTcl

# LDFLAGS = -L. -L../SpecTcl -L../Display -L../Gates -L../Sorter -L../Xamine -L../Events -L../EventSource -L../TCL -L../NSCLException -L../SpectrumIO -L../Replace -L../Tape 

bin_PROGRAMS = testfile ltwrite

testfile_SOURCES = testfile.cpp
#   testfile_LDFLAGS = $(LDFLAGS)

ltwrite_SOURCES = ltwrite.cpp
#   ltwrite_LDFLAGS = $(LDFLAGS)
