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

DisplayGate.o: DisplayGate.h Point.h /Include/histotypes.h
DisplayGate.o: /usr/include/string.h /usr/include/features.h
DisplayGate.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
DisplayGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
DisplayGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
DisplayGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
DisplayGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
DisplayGate.o: /Include/RangeError.h /Include/Exception.h
DisplayGate.o: /usr/include/assert.h
Xamine1D.o: Xamine1D.h XamineSpectrum.h /Include/histotypes.h
Xamine1D.o: /usr/include/string.h /usr/include/features.h
Xamine1D.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Xamine1D.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Xamine1D.o: /usr/include/math.h /usr/include/bits/huge_val.h
Xamine1D.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Xamine1D.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
Xamine2D.o: Xamine2D.h XamineSpectrum.h /Include/histotypes.h
Xamine2D.o: /usr/include/string.h /usr/include/features.h
Xamine2D.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Xamine2D.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Xamine2D.o: /usr/include/math.h /usr/include/bits/huge_val.h
Xamine2D.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Xamine2D.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
Xamine2D.o: /usr/include/assert.h
XamineButton.o: XamineButton.h /Include/histotypes.h /usr/include/string.h
XamineButton.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineButton.o: /usr/include/gnu/stubs.h
XamineButton.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineButton.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineButton.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
XamineButton.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineButton.o: XamineButtonPrompt.h /Include/RangeError.h
XamineButton.o: /Include/Exception.h XaminePushButton.h XamineToggleButton.h
XamineButtonException.o: XamineButtonException.h /Include/Exception.h
XamineButtonException.o: /Include/histotypes.h /usr/include/string.h
XamineButtonException.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineButtonException.o: /usr/include/gnu/stubs.h
XamineButtonException.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineButtonException.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineButtonException.o: /usr/include/bits/mathdef.h
XamineButtonException.o: /usr/include/bits/mathcalls.h
XamineButtonException.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineButtonException.o: XamineButton.h XaminePushButton.h
XamineButtonException.o: XamineToggleButton.h /usr/include/assert.h
XamineButtonPrompt.o: XamineButtonPrompt.h /Include/RangeError.h
XamineButtonPrompt.o: /Include/Exception.h
XamineConfirmPrompt.o: XamineConfirmPrompt.h XamineButtonPrompt.h
XamineConfirmPrompt.o: /Include/RangeError.h /Include/Exception.h
XamineConfirmPrompt.o: /usr/include/string.h /usr/include/features.h
XamineConfirmPrompt.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineConfirmPrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineConfirmPrompt.o: /usr/include/assert.h
XamineEvent.o: XamineEvent.h DisplayGate.h Point.h /Include/histotypes.h
XamineEvent.o: /usr/include/string.h /usr/include/features.h
XamineEvent.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineEvent.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineEvent.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineEvent.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
XamineEvent.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineEvent.o: ButtonEvent.h
XamineFilePrompt.o: XamineFilePrompt.h XamineButtonPrompt.h
XamineFilePrompt.o: /Include/histotypes.h /usr/include/string.h
XamineFilePrompt.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineFilePrompt.o: /usr/include/gnu/stubs.h
XamineFilePrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineFilePrompt.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineFilePrompt.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
XamineFilePrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineGateException.o: XamineGateException.h /Include/Exception.h
XamineGateException.o: DisplayGate.h Point.h /Include/histotypes.h
XamineGateException.o: /usr/include/string.h /usr/include/features.h
XamineGateException.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineGateException.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineGateException.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineGateException.o: /usr/include/bits/mathdef.h
XamineGateException.o: /usr/include/bits/mathcalls.h
XamineGateException.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineGateException.o: /usr/include/assert.h
XamineGates.o: XamineGates.h DisplayGate.h Point.h /Include/histotypes.h
XamineGates.o: /usr/include/string.h /usr/include/features.h
XamineGates.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineGates.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineGates.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineGates.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
XamineGates.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineGates.o: /Include/RangeError.h /Include/Exception.h
XamineGates.o: /usr/include/assert.h
XamineNoPrompt.o: XamineNoPrompt.h XamineButtonPrompt.h
XaminePointsPrompt.o: XaminePointsPrompt.h XamineButtonPrompt.h
XaminePointsPrompt.o: /Include/histotypes.h /usr/include/string.h
XaminePointsPrompt.o: /usr/include/features.h /usr/include/sys/cdefs.h
XaminePointsPrompt.o: /usr/include/gnu/stubs.h
XaminePointsPrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XaminePointsPrompt.o: /usr/include/math.h /usr/include/bits/huge_val.h
XaminePointsPrompt.o: /usr/include/bits/mathdef.h
XaminePointsPrompt.o: /usr/include/bits/mathcalls.h
XaminePointsPrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XaminePushButton.o: XaminePushButton.h
XamineSpectrum.o: XamineSpectrum.h /Include/histotypes.h
XamineSpectrum.o: /usr/include/string.h /usr/include/features.h
XamineSpectrum.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineSpectrum.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineSpectrum.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
XamineSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineSpectrum.o: /usr/include/assert.h
XamineSpectrumIterator.o: XamineSpectrumIterator.h /Include/histotypes.h
XamineSpectrumIterator.o: /usr/include/string.h /usr/include/features.h
XamineSpectrumIterator.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineSpectrumIterator.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineSpectrumIterator.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineSpectrumIterator.o: /usr/include/bits/mathdef.h
XamineSpectrumIterator.o: /usr/include/bits/mathcalls.h
XamineSpectrumIterator.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineSpectrumIterator.o: XamineSpectrum.h Xamine1D.h Xamine2D.h
XamineSpectrumPrompt.o: XamineSpectrumPrompt.h XamineButtonPrompt.h
XamineSpectrumPrompt.o: /Include/histotypes.h /usr/include/string.h
XamineSpectrumPrompt.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineSpectrumPrompt.o: /usr/include/gnu/stubs.h
XamineSpectrumPrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineSpectrumPrompt.o: /usr/include/math.h /usr/include/bits/huge_val.h
XamineSpectrumPrompt.o: /usr/include/bits/mathdef.h
XamineSpectrumPrompt.o: /usr/include/bits/mathcalls.h
XamineSpectrumPrompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
XamineSpectrumPrompt.o: /usr/include/assert.h
XamineTextPrompt.o: XamineTextPrompt.h XamineButtonPrompt.h
XamineToggleButton.o: XamineToggleButton.h
Xamineplus.o: Xamineplus.h /Include/histotypes.h /usr/include/string.h
Xamineplus.o: /usr/include/features.h /usr/include/sys/cdefs.h
Xamineplus.o: /usr/include/gnu/stubs.h
Xamineplus.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Xamineplus.o: /usr/include/math.h /usr/include/bits/huge_val.h
Xamineplus.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Xamineplus.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
Xamineplus.o: XamineSpectrumIterator.h XamineGates.h DisplayGate.h Point.h
Xamineplus.o: /Include/ErrnoException.h /Include/Exception.h
Xamineplus.o: /usr/include/errno.h /usr/include/bits/errno.h
Xamineplus.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Xamineplus.o: /Include/RangeError.h XamineGateException.h
Xamineplus.o: XamineButtonException.h XamineButton.h XamineEvent.h
Xamineplus.o: XamineSpectrum.h Xamine1D.h Xamine2D.h XamineNoPrompt.h
Xamineplus.o: XamineButtonPrompt.h XamineConfirmPrompt.h XamineFilePrompt.h
Xamineplus.o: XaminePointsPrompt.h XamineTextPrompt.h XamineSpectrumPrompt.h
Xamineplus.o: /usr/include/assert.h
Xaminetest.o: /Include/histotypes.h /usr/include/string.h
Xaminetest.o: /usr/include/features.h /usr/include/sys/cdefs.h
Xaminetest.o: /usr/include/gnu/stubs.h
Xaminetest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Xaminetest.o: /usr/include/math.h /usr/include/bits/huge_val.h
Xaminetest.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Xaminetest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h Xamineplus.h
Xaminetest.o: XamineSpectrumIterator.h XamineSpectrumPrompt.h
Xaminetest.o: XamineButtonPrompt.h XaminePushButton.h XamineNoPrompt.h
Xaminetest.o: XamineEvent.h XamineGates.h DisplayGate.h Point.h ButtonEvent.h
Xaminetest.o: Xamine1D.h XamineSpectrum.h Xamine2D.h /usr/include/sys/wait.h
Xaminetest.o: /usr/include/bits/types.h /usr/include/bits/waitflags.h
Xaminetest.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
Xaminetest.o: /usr/include/bits/endian.h /usr/include/bits/siginfo.h
