# INCLUDES = -I. -I../Utility -I../EventSource -I../Tape -I../Exception
INCLUDES= -I. -I../EventSource -I../TCL -I../Exception -I../Utility @TCL_FLAGS@ -I../Tape
# LDADD = -lTclGrammerCommands -lXamine -lGates -lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException -lSpecio -lreplace -ltape -lm
LDADD = -lAnalysis -lEventSource -ltclPlus -lException -ltape

LDFLAGS = -L. -L../Events -L../EventSource -L../TCL -L../Exception -L../Tape
# LDFLAGS = -L. -L../SpecTcl -L../Display -L../Gates -L../Sorter -L../Xamine -L../Events -L../EventSource -L../TCL -L../Exception -L../SpectrumIO -L../Replace -L../Tape

bin_PROGRAMS = testfile ltwrite

testfile_SOURCES = testfile.cpp
#   testfile_LDFLAGS = $(LDFLAGS)

ltwrite_SOURCES = ltwrite.cpp
#   ltwrite_LDFLAGS = $(LDFLAGS)
