#
#  Makefile for the TCL Controlled  Histogramming program.
#  targets include:
#     system(default)             - Builds the software.
#     clean                       - Kills off .o, .a files.
#     depend                      - Rebuilds the dependencies with makedepend.
#                                   in current directory.
#
#
INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include \
	-I/usr/local/daq/include
LIBDIRS= -L. -L$(INSTDIR)/Lib -L/usr/local -L/usr/local/daq/lib 
LIBS=   -lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException \
	 -ltape -lm

ECHO=echo
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS)
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar -r -c

system: testfile ltwrite

clean: 
	$(RM) testfile ltwrite


install: system


depend:
	$(DEPEND) $(INCLUDEDIRS) *.cpp

testfile: testfile.cpp
	$(LINK) -o testfile   testfile.cpp  $(LIBS)

ltwrite: ltwrite.cpp
	$(LINK) -o ltwrite ltwrite.cpp $(LIBS)
# DO NOT DELETE
