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

INCLUDEDIRS=-I/usr/local/daq/include -I$(INSTDIR)/Include -I.
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
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr

system:	libXplus.a 
	$(ECHO) "Library rebuild complete."

clean:
	$(RM) $(OBJECTS) libXplus.a

depend:	
	$(DEPEND)  $(INCLUDEDIRS) *.cpp

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
# DO NOT DELETE

DisplayGate.o: DisplayGate.h Point.h /usr/include/string.h
DisplayGate.o: /usr/include/features.h /usr/include/sys/cdefs.h
DisplayGate.o: /usr/include/gnu/stubs.h
DisplayGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
DisplayGate.o: /usr/include/assert.h
Xamine1D.o: Xamine1D.h XamineSpectrum.h
Xamine2D.o: Xamine2D.h XamineSpectrum.h /usr/include/assert.h
Xamine2D.o: /usr/include/features.h /usr/include/sys/cdefs.h
Xamine2D.o: /usr/include/gnu/stubs.h
XamineButton.o: XamineButton.h XamineButtonPrompt.h XaminePushButton.h
XamineButton.o: XamineToggleButton.h /usr/include/string.h
XamineButton.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineButton.o: /usr/include/gnu/stubs.h
XamineButton.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineButtonException.o: XamineButtonException.h XamineButton.h
XamineButtonException.o: XaminePushButton.h XamineToggleButton.h
XamineButtonException.o: /usr/include/assert.h /usr/include/features.h
XamineButtonException.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineButtonPrompt.o: XamineButtonPrompt.h
XamineConfirmPrompt.o: XamineConfirmPrompt.h XamineButtonPrompt.h
XamineConfirmPrompt.o: /usr/include/string.h /usr/include/features.h
XamineConfirmPrompt.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineConfirmPrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineConfirmPrompt.o: /usr/include/assert.h
XamineEvent.o: XamineEvent.h DisplayGate.h Point.h ButtonEvent.h
XamineFilePrompt.o: XamineFilePrompt.h XamineButtonPrompt.h
XamineGateException.o: XamineGateException.h DisplayGate.h Point.h
XamineGateException.o: /usr/include/assert.h /usr/include/features.h
XamineGateException.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineGates.o: XamineGates.h DisplayGate.h Point.h /usr/include/assert.h
XamineGates.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineGates.o: /usr/include/gnu/stubs.h
XamineNoPrompt.o: XamineNoPrompt.h XamineButtonPrompt.h
XaminePointsPrompt.o: XaminePointsPrompt.h XamineButtonPrompt.h
XaminePushButton.o: XaminePushButton.h
XamineSpectrum.o: XamineSpectrum.h /usr/include/string.h
XamineSpectrum.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineSpectrum.o: /usr/include/gnu/stubs.h
XamineSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineSpectrum.o: /usr/include/assert.h
XamineSpectrumIterator.o: XamineSpectrumIterator.h XamineSpectrum.h
XamineSpectrumIterator.o: Xamine1D.h Xamine2D.h
XamineSpectrumPrompt.o: XamineSpectrumPrompt.h XamineButtonPrompt.h
XamineSpectrumPrompt.o: /usr/include/assert.h /usr/include/features.h
XamineSpectrumPrompt.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineTextPrompt.o: XamineTextPrompt.h XamineButtonPrompt.h
XamineToggleButton.o: XamineToggleButton.h
Xamineplus.o: Xamineplus.h XamineSpectrumIterator.h XamineGates.h
Xamineplus.o: DisplayGate.h Point.h XamineGateException.h
Xamineplus.o: XamineButtonException.h XamineButton.h XamineEvent.h
Xamineplus.o: XamineSpectrum.h Xamine1D.h Xamine2D.h XamineNoPrompt.h
Xamineplus.o: XamineButtonPrompt.h XamineConfirmPrompt.h XamineFilePrompt.h
Xamineplus.o: XaminePointsPrompt.h XamineTextPrompt.h XamineSpectrumPrompt.h
Xamineplus.o: /usr/include/errno.h /usr/include/features.h
Xamineplus.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Xamineplus.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
Xamineplus.o: /usr/include/asm/errno.h /usr/include/assert.h
Xaminetest.o: Xamineplus.h XamineSpectrumIterator.h XamineSpectrumPrompt.h
Xaminetest.o: XamineButtonPrompt.h XaminePushButton.h XamineNoPrompt.h
Xaminetest.o: XamineEvent.h XamineGates.h DisplayGate.h Point.h ButtonEvent.h
Xaminetest.o: Xamine1D.h XamineSpectrum.h Xamine2D.h /usr/include/sys/wait.h
Xaminetest.o: /usr/include/features.h /usr/include/sys/cdefs.h
Xaminetest.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
Xaminetest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Xaminetest.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
Xaminetest.o: /usr/include/endian.h /usr/include/bits/endian.h
Xaminetest.o: /usr/include/bits/siginfo.h
