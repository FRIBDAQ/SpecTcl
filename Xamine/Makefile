#
#    Makefile for the Xamine API Package of the histogrammer.
#    The folowing are the important targets:
#        system [default]     - Makes libXplus.a library.
#        install              - Installs to $(INSTDIR) tree.
#        clean                - Cleans up objects etc.
#        depend               - Remakes the dependency list with makedepend.
#        tests                - Creates test program(s)
# Author:
#   Ron Fox
#   NSCL
#   Michigan State University
#   East Lansing, MI 48824-1321
#   (c) Copyright NSCL 1999 All rights reserved.
#

DarwinINCS=-I/usr/include/gcc/darwin/2.95.2/g++

INCLUDEDIRS=-I$(INSTDIR)/Include -I. $($(OS)INCS)
OBJECTS = DisplayGate.o Xamine1D.o Xamine2D.o XamineButton.o \
	XamineButtonException.o XamineButtonPrompt.o XamineConfirmPrompt.o \
	XamineEvent.o XamineFilePrompt.o XamineGateException.o XamineGates.o \
	XamineNoPrompt.o XaminePointsPrompt.o \
	XamineSpectrumPrompt.o  XaminePushButton.o \
	XamineSpectrum.o XamineSpectrumIterator.o \
	XamineTextPrompt.o XamineToggleButton.o Xamineplus.o
LIBDIRS=-L/usr/local/daq/lib -L. -L$(INSTDIR)/Lib

LIBS= -lXplus -lXamine -lException


ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr

system:	libXplus.a 
	$(ECHO) "Library rebuild complete."

clean:
	$(RM) $(OBJECTS) libXplus.a

depend:	
	$(DEPEND)  -Y. *.cpp

install:
	$(INSTALL) libXplus.a $(INSTDIR)/Lib
	$(INSTALL) *.h        $(INSTDIR)/Include

tests: Xaminetest
	$(ECHO) "Test program built"

libXplus.a: $(OBJECTS)
	$(ARCHIVE) libXplus.a $(OBJECTS)


Xaminetest: libXplus.a Xaminetest.cpp
	$(LINK) -o Xaminetest Xaminetest.cpp $(LIBS)

DisplayGate.o: DisplayGate.cpp
	$(COMPILE)  -o $@ DisplayGate.cpp
 
Xamine1D.o: Xamine1D.cpp
	$(COMPILE)  -o $@ Xamine1D.cpp

Xamine2D.o: Xamine2D.cpp 
	$(COMPILE)  -o $@ Xamine2D.cpp

XamineButton.o: XamineButton.cpp
	$(COMPILE)  -o $@ XamineButton.cpp

XamineButtonException.o: XamineButtonException.cpp
	$(COMPILE)  -o $@ XamineButtonException.cpp

XamineButtonPrompt.o: XamineButtonPrompt.cpp 
	$(COMPILE)  -o $@ XamineButtonPrompt.cpp

XamineConfirmPrompt.o: XamineConfirmPrompt.cpp
	$(COMPILE)  -o $@ XamineConfirmPrompt.cpp

XamineEvent.o: XamineEvent.cpp 
	$(COMPILE)  -o $@ XamineEvent.cpp

XamineFilePrompt.o: XamineFilePrompt.cpp 
	$(COMPILE)  -o $@ XamineFilePrompt.cpp

XamineGateException.o: XamineGateException.cpp 
	$(COMPILE)  -o $@ XamineGateException.cpp

XamineGates.o: XamineGates.cpp
	$(COMPILE)  -o $@ XamineGates.cpp

XamineNoPrompt.o: XamineNoPrompt.cpp 
	$(COMPILE)  -o $@ XamineNoPrompt.cpp

XaminePointsPrompt.o: XaminePointsPrompt.cpp 
	$(COMPILE)  -o $@ XaminePointsPrompt.cpp

XaminePushButton.o: XaminePushButton.cpp
	$(COMPILE)  -o $@ XaminePushButton.cpp

XamineSpectrum.o: XamineSpectrum.cpp 
	$(COMPILE)  -o $@ XamineSpectrum.cpp

XamineSpectrumIterator.o: XamineSpectrumIterator.cpp
	$(COMPILE)  -o $@ XamineSpectrumIterator.cpp

XamineTextPrompt.o: XamineTextPrompt.cpp 
	$(COMPILE)  -o $@ XamineTextPrompt.cpp

XamineToggleButton.o: XamineToggleButton.cpp 
	$(COMPILE)  -o $@ XamineToggleButton.cpp

Xamineplus.o: Xamineplus.cpp
	$(COMPILE)  -o $@ Xamineplus.cpp

XamineSpectrumPrompt.o: XamineSpectrumPrompt.cpp
	$(COMPILE) XamineSpectrumPrompt.cpp
