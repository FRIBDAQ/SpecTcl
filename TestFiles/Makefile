#
#  Makefile for the TCL Controlled  Histogramming program.
#  targets include:
#     system(default)             - Builds the software.
#     clean                       - Kills off .o, .a files.
#     depend                      - Rebuilds the dependencies with makedepend.
#                                   in current directory.
#
#
SPECTCL_VERSION = -DVERSION=$(VERSION)
INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include
LIBDIRS= -L. -L$(INSTDIR)/Lib -L/usr/local
LIBS=   -lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException \
	 -ltape -lm

ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) $(SPECTCL_VERSION)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) $(SPECTCL_VERSION)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar -r -c

system: testfile ltwrite

clean: 
	$(RM) testfile ltwrite


install: system


depend:
	$(DEPEND) -Y.  *.cpp

testfile: testfile.cpp
	$(LINK)  -o testfile   testfile.cpp  $(LIBS)

ltwrite: ltwrite.cpp
	$(LINK)  -o ltwrite ltwrite.cpp $(LIBS)
