INCLUDES = -I. -I../Utility -I../EventSource -I../Tape -I../Exception
LDADD = -lTclGrammerCommands -lXamine -lGates -lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException -lSpecio -lreplace -ltape -lm
LDFLAGS = -L. -L../SpecTcl -L../Display -L../Gates -L../Sorter -L../Xamine -L../Events -L../EventSource -L../TCL -L../Exception -L../SpectrumIO -L../Replace -L../Tape

bin_PROGRAMS = testfile ltwrite

testfile_SOURCES = testfile.cpp
testfile_LDADD = ../SpecTcl/MySpecTclApp.o $(LDADD)
testfile_LDFLAGS = $(LDFLAGS)

ltwrite_SOURCES = ltwrite.cpp
ltwrite_LDADD = ../SpecTcl/MySpecTclApp.o $(LDADD)
ltwrite_LDFLAGS = $(LDFLAGS)
