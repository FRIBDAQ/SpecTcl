INSTDIR=/opt/spectcl/0.4
OS=Linux
#
#   Makefile to build user tailored SpecTcl.
#

LinuxLIBS=-ldl
HPUXLIBS=-L/opt/tcltk/lib
HPUXINCS=-I/opt/tcltk/include


#
#  Comment the next line and uncomment the following to enable profiling.
PROFILE=-g
# PROFILE=-g3 -pg

OBJECTS=a1900SpecTcl.o a1900Unpacker.o a1900Calibrator.o
INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include \
	-I/usr/local/daq/include $($(OS)INCS)
LIBDIRS= -L$(INSTDIR)/Lib -L/usr/local -L/usr/local/daq/lib -L/usr/X11/lib
LIBS=   -lTclGrammerCommands \
	-lSorting -lXplus -lGates -lAnalysis -lEventSource -lSpecio \
	-ltclPlus -lException \
	-lXamine -ltape \
	-ltk -ltcl -lXt -lX11 -lm $($(OS)LIBS)
ECHO=echo
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr
CONCAT=cat

SpecTcl: $(OBJECTS)
	$(LINK) $(LIBDIRS) -o SpecTcl $(INSTDIR)/Lib/Globals.o $(OBJECTS) \
	 $(LIBS)

a1900Unpacker.o: a1900Unpacker.cpp
	$(COMPILE) a1900Unpacker.cpp

a1900Calibrator.o: a1900Calibrator.cpp
	$(COMPILE) a1900Calibrator.cpp

a1900SpecTcl.o: a1900SpecTcl.cpp
	$(COMPILE) a1900SpecTcl.cpp

