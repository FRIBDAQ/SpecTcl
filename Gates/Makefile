#
#  Makefile for the gates object library (SpecTcl facility).
#


LinuxLIBS=-ldl
CYGWININCS=-I/usr/local/tcl/include
CYGWINLIBDIR=-L/usr/local/tcl/lib

INCLUDEDIRS= $($(OS)INCS) -I. -I$(INSTDIR)/Include -I/usr/local/include -I/usr/local/daq/include
LIBDIRS= $($(OS)LIBDIR) \
	-L. -L$(INSTDIR)/Lib -L/usr/local -L/usr/local/daq/lib -L/usr/X11/lib
LIBS=   -lTclGrammerCommands \
	-lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException \
	-lXamine -ltape \
	-ltk -ltcl -lXt -lX11 -lm $($(OS)LIBS)

ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
LINK=$($(OS)LD) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr
CONCAT=cat
OBJECTS =  ActualIterator.o AndGate.o Band.o CompoundGate.o \
	ConstituentIterator.o Contour.o Cut.o DeletedGate.o \
	FalseGate.o Gate.o GateContainer.o GateListIterator.o \
	Interior.o InteriorRow.o Not.o OrGate.o PointlistGate.o \
	PointlistIterator.o SingleItemIterator.o TrueGate.o C2Bands.o \
	GammaCut.o GammaBand.o GammaContour.o


system: libGates.a

clean: 
	$(RM) $(OBJECTS)
	$(RM) libGates.a


install: system
	$(INSTALL) libGates.a $(INSTDIR)/Lib
	$(INSTALL) *.h        $(INSTDIR)/Include

depend:
	$(DEPEND) $(INCLUDEDIRS) *.cpp

libGates.a: $(OBJECTS)
	$(ARCHIVE) libGates.a $(OBJECTS) 
	ranlib     libGates.a

ActualIterator.o: ActualIterator.cpp
	$(COMPILE) ActualIterator.cpp

AndGate.o: AndGate.cpp
	$(COMPILE) AndGate.cpp

Band.o: Band.cpp
	$(COMPILE) Band.cpp

CompoundGate.o: CompoundGate.cpp
	$(COMPILE) CompoundGate.cpp

ConstituentIterator.o: ConstituentIterator.cpp
	$(COMPILE) ConstituentIterator.cpp

Contour.o: Contour.cpp
	$(COMPILE) Contour.cpp

Cut.o: Cut.cpp
	$(COMPILE) Cut.cpp

DeletedGate.o: DeletedGate.cpp
	$(COMPILE) DeletedGate.cpp

FalseGate.o: FalseGate.cpp
	$(COMPILE) FalseGate.cpp

Gate.o: Gate.cpp
	$(COMPILE) Gate.cpp

GateContainer.o: GateContainer.cpp
	$(COMPILE) GateContainer.cpp

GateListIterator.o: GateListIterator.cpp
	$(COMPILE) GateListIterator.cpp

Interior.o: Interior.cpp
	$(COMPILE) Interior.cpp

InteriorRow.o: InteriorRow.cpp
	$(COMPILE) InteriorRow.cpp

Not.o: Not.cpp
	$(COMPILE) Not.cpp

OrGate.o: OrGate.cpp
	$(COMPILE) OrGate.cpp

PointlistGate.o: PointlistGate.cpp
	$(COMPILE) PointlistGate.cpp

PointlistIterator.o: PointlistIterator.cpp
	$(COMPILE) PointlistIterator.cpp

SingleItemIterator.o: SingleItemIterator.cpp
	$(COMPILE) SingleItemIterator.cpp

TrueGate.o: TrueGate.cpp
	$(COMPILE) TrueGate.cpp


C2Bands.o: C2Bands.cpp
	$(COMPILE) C2Bands.cpp

GammaCut.o: GammaCut.cpp
	$(COMPILE) GammaCut.cpp

GammaBand.o: GammaBand.cpp
	$(COMPILE) GammaBand.cpp

GammaContour.o: GammaContour.cpp
	$(COMPILE) GammaContour.cpp
# DO NOT DELETE

ActualIterator.o: ActualIterator.h /usr/include/assert.h
ActualIterator.o: /usr/include/features.h /usr/include/sys/cdefs.h
ActualIterator.o: /usr/include/gnu/stubs.h
AndGate.o: AndGate.h CompoundGate.h Gate.h ConstituentIterator.h
AndGate.o: ActualIterator.h /opt/spectcl/test/Include/histotypes.h
AndGate.o: /usr/include/string.h /usr/include/features.h
AndGate.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
AndGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
AndGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
AndGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
AndGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h GateContainer.h
AndGate.o: /opt/spectcl/test/Include/NamedItem.h
Band.o: Band.h PointlistGate.h Gate.h ConstituentIterator.h ActualIterator.h
Band.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
Band.o: /usr/include/features.h /usr/include/sys/cdefs.h
Band.o: /usr/include/gnu/stubs.h
Band.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Band.o: /usr/include/math.h /usr/include/bits/huge_val.h
Band.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Band.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
Band.o: /usr/include/assert.h
C2Bands.o: C2Bands.h Contour.h PointlistGate.h Gate.h ConstituentIterator.h
C2Bands.o: ActualIterator.h /opt/spectcl/test/Include/histotypes.h
C2Bands.o: /usr/include/string.h /usr/include/features.h
C2Bands.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
C2Bands.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
C2Bands.o: /usr/include/math.h /usr/include/bits/huge_val.h
C2Bands.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
C2Bands.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h Interior.h
C2Bands.o: InteriorRow.h Band.h
CompoundGate.o: CompoundGate.h Gate.h ConstituentIterator.h ActualIterator.h
CompoundGate.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
CompoundGate.o: /usr/include/features.h /usr/include/sys/cdefs.h
CompoundGate.o: /usr/include/gnu/stubs.h
CompoundGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
CompoundGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
CompoundGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
CompoundGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
CompoundGate.o: GateContainer.h /opt/spectcl/test/Include/NamedItem.h
CompoundGate.o: GateListIterator.h
ConstituentIterator.o: ConstituentIterator.h ActualIterator.h
Contour.o: Contour.h PointlistGate.h Gate.h ConstituentIterator.h
Contour.o: ActualIterator.h /opt/spectcl/test/Include/histotypes.h
Contour.o: /usr/include/string.h /usr/include/features.h
Contour.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Contour.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Contour.o: /usr/include/math.h /usr/include/bits/huge_val.h
Contour.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Contour.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h Interior.h
Contour.o: InteriorRow.h
Cut.o: Cut.h Gate.h ConstituentIterator.h ActualIterator.h
Cut.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
Cut.o: /usr/include/features.h /usr/include/sys/cdefs.h
Cut.o: /usr/include/gnu/stubs.h
Cut.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Cut.o: /usr/include/math.h /usr/include/bits/huge_val.h
Cut.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Cut.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
Cut.o: SingleItemIterator.h /usr/include/stdio.h
Cut.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
Cut.o: /usr/include/bits/types.h /usr/include/libio.h
Cut.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
DeletedGate.o: DeletedGate.h FalseGate.h Gate.h ConstituentIterator.h
DeletedGate.o: ActualIterator.h /opt/spectcl/test/Include/histotypes.h
DeletedGate.o: /usr/include/string.h /usr/include/features.h
DeletedGate.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
DeletedGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
DeletedGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
DeletedGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
DeletedGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
FalseGate.o: FalseGate.h Gate.h ConstituentIterator.h ActualIterator.h
FalseGate.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
FalseGate.o: /usr/include/features.h /usr/include/sys/cdefs.h
FalseGate.o: /usr/include/gnu/stubs.h
FalseGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
FalseGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
FalseGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
FalseGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
FalseGate.o: SingleItemIterator.h
GammaBand.o: GammaBand.h Band.h PointlistGate.h Gate.h ConstituentIterator.h
GammaBand.o: ActualIterator.h /opt/spectcl/test/Include/histotypes.h
GammaBand.o: /usr/include/string.h /usr/include/features.h
GammaBand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GammaBand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
GammaBand.o: /usr/include/math.h /usr/include/bits/huge_val.h
GammaBand.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
GammaBand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
GammaBand.o: /usr/include/assert.h
GammaContour.o: GammaContour.h Contour.h PointlistGate.h Gate.h
GammaContour.o: ConstituentIterator.h ActualIterator.h
GammaContour.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
GammaContour.o: /usr/include/features.h /usr/include/sys/cdefs.h
GammaContour.o: /usr/include/gnu/stubs.h
GammaContour.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
GammaContour.o: /usr/include/math.h /usr/include/bits/huge_val.h
GammaContour.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
GammaContour.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h Interior.h
GammaContour.o: InteriorRow.h
GammaCut.o: GammaCut.h Cut.h Gate.h ConstituentIterator.h ActualIterator.h
GammaCut.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
GammaCut.o: /usr/include/features.h /usr/include/sys/cdefs.h
GammaCut.o: /usr/include/gnu/stubs.h
GammaCut.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
GammaCut.o: /usr/include/math.h /usr/include/bits/huge_val.h
GammaCut.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
GammaCut.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
GammaCut.o: SingleItemIterator.h /usr/include/stdio.h
GammaCut.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
GammaCut.o: /usr/include/bits/types.h /usr/include/libio.h
GammaCut.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
Gate.o: Gate.h ConstituentIterator.h ActualIterator.h
Gate.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
Gate.o: /usr/include/features.h /usr/include/sys/cdefs.h
Gate.o: /usr/include/gnu/stubs.h
Gate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Gate.o: /usr/include/math.h /usr/include/bits/huge_val.h
Gate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Gate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
GateContainer.o: GateContainer.h /opt/spectcl/test/Include/NamedItem.h
GateContainer.o: /usr/include/string.h /usr/include/features.h
GateContainer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GateContainer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
GateListIterator.o: GateListIterator.h
Interior.o: Interior.h InteriorRow.h /usr/include/assert.h
Interior.o: /usr/include/features.h /usr/include/sys/cdefs.h
Interior.o: /usr/include/gnu/stubs.h /usr/include/stdio.h
Interior.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Interior.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
Interior.o: /usr/include/bits/types.h /usr/include/libio.h
Interior.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
InteriorRow.o: InteriorRow.h
Not.o: Not.h GateContainer.h /opt/spectcl/test/Include/NamedItem.h
Not.o: /usr/include/string.h /usr/include/features.h /usr/include/sys/cdefs.h
Not.o: /usr/include/gnu/stubs.h
Not.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h Gate.h
Not.o: ConstituentIterator.h ActualIterator.h
Not.o: /opt/spectcl/test/Include/histotypes.h /usr/include/math.h
Not.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Not.o: /usr/include/bits/mathcalls.h
Not.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
Not.o: SingleItemIterator.h
OrGate.o: OrGate.h CompoundGate.h Gate.h ConstituentIterator.h
OrGate.o: ActualIterator.h /opt/spectcl/test/Include/histotypes.h
OrGate.o: /usr/include/string.h /usr/include/features.h
OrGate.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
OrGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
OrGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
OrGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
OrGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h GateContainer.h
OrGate.o: /opt/spectcl/test/Include/NamedItem.h
PointlistGate.o: PointlistGate.h Gate.h ConstituentIterator.h
PointlistGate.o: ActualIterator.h /opt/spectcl/test/Include/histotypes.h
PointlistGate.o: /usr/include/string.h /usr/include/features.h
PointlistGate.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
PointlistGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
PointlistGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
PointlistGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
PointlistGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
PointlistGate.o: PointlistIterator.h /usr/include/stdio.h
PointlistGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
PointlistGate.o: /usr/include/bits/types.h /usr/include/libio.h
PointlistGate.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
PointlistIterator.o: PointlistIterator.h
SingleItemIterator.o: SingleItemIterator.h
SingleItemIterator.o: /opt/spectcl/test/Include/histotypes.h
SingleItemIterator.o: /usr/include/string.h /usr/include/features.h
SingleItemIterator.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
SingleItemIterator.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
SingleItemIterator.o: /usr/include/math.h /usr/include/bits/huge_val.h
SingleItemIterator.o: /usr/include/bits/mathdef.h
SingleItemIterator.o: /usr/include/bits/mathcalls.h
SingleItemIterator.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
TrueGate.o: TrueGate.h Gate.h ConstituentIterator.h ActualIterator.h
TrueGate.o: /opt/spectcl/test/Include/histotypes.h /usr/include/string.h
TrueGate.o: /usr/include/features.h /usr/include/sys/cdefs.h
TrueGate.o: /usr/include/gnu/stubs.h
TrueGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
TrueGate.o: /usr/include/math.h /usr/include/bits/huge_val.h
TrueGate.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
TrueGate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
TrueGate.o: SingleItemIterator.h
