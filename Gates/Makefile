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
ActualIterator.o: /usr/include/standards.h
AndGate.o: AndGate.h CompoundGate.h Gate.h ConstituentIterator.h
AndGate.o: ActualIterator.h GateContainer.h
Band.o: Band.h PointlistGate.h Gate.h ConstituentIterator.h ActualIterator.h
Band.o: /usr/include/assert.h /usr/include/standards.h
C2Bands.o: C2Bands.h Contour.h PointlistGate.h Gate.h ConstituentIterator.h
C2Bands.o: ActualIterator.h Interior.h InteriorRow.h Band.h
CompoundGate.o: CompoundGate.h Gate.h ConstituentIterator.h ActualIterator.h
CompoundGate.o: GateContainer.h GateListIterator.h
ConstituentIterator.o: ConstituentIterator.h ActualIterator.h
Contour.o: Contour.h PointlistGate.h Gate.h ConstituentIterator.h
Contour.o: ActualIterator.h Interior.h InteriorRow.h
Cut.o: Cut.h Gate.h ConstituentIterator.h ActualIterator.h
Cut.o: SingleItemIterator.h /usr/include/stdio.h /usr/include/standards.h
Cut.o: /usr/include/sys/seek.h /usr/include/va_list.h
Cut.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
Cut.o: /usr/include/sys/select.h /usr/include/getopt.h
Cut.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
Cut.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
Cut.o: /usr/include/sys/rt_limits.h
DeletedGate.o: DeletedGate.h FalseGate.h Gate.h ConstituentIterator.h
DeletedGate.o: ActualIterator.h
FalseGate.o: FalseGate.h Gate.h ConstituentIterator.h ActualIterator.h
FalseGate.o: SingleItemIterator.h
GammaBand.o: GammaBand.h Band.h PointlistGate.h Gate.h ConstituentIterator.h
GammaBand.o: ActualIterator.h /usr/include/assert.h /usr/include/standards.h
GammaContour.o: GammaContour.h Contour.h PointlistGate.h Gate.h
GammaContour.o: ConstituentIterator.h ActualIterator.h Interior.h
GammaContour.o: InteriorRow.h
GammaCut.o: GammaCut.h Cut.h Gate.h ConstituentIterator.h ActualIterator.h
GammaCut.o: SingleItemIterator.h /usr/include/stdio.h
GammaCut.o: /usr/include/standards.h /usr/include/sys/seek.h
GammaCut.o: /usr/include/va_list.h /usr/include/sys/types.h
GammaCut.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
GammaCut.o: /usr/include/getopt.h /usr/include/sys/limits.h
GammaCut.o: /usr/include/sys/machine/machlimits.h
GammaCut.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
GammaCut.o: /usr/include/sys/rt_limits.h
Gate.o: Gate.h ConstituentIterator.h ActualIterator.h
GateContainer.o: GateContainer.h
GateListIterator.o: GateListIterator.h
Interior.o: Interior.h InteriorRow.h /usr/include/assert.h
Interior.o: /usr/include/standards.h /usr/include/stdio.h
Interior.o: /usr/include/sys/seek.h /usr/include/va_list.h
Interior.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
Interior.o: /usr/include/sys/select.h /usr/include/getopt.h
Interior.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
Interior.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
Interior.o: /usr/include/sys/rt_limits.h
InteriorRow.o: InteriorRow.h
Not.o: Not.h GateContainer.h Gate.h ConstituentIterator.h ActualIterator.h
Not.o: SingleItemIterator.h
OrGate.o: OrGate.h CompoundGate.h Gate.h ConstituentIterator.h
OrGate.o: ActualIterator.h GateContainer.h
PointlistGate.o: PointlistGate.h Gate.h ConstituentIterator.h
PointlistGate.o: ActualIterator.h PointlistIterator.h /usr/include/stdio.h
PointlistGate.o: /usr/include/standards.h /usr/include/sys/seek.h
PointlistGate.o: /usr/include/va_list.h /usr/include/sys/types.h
PointlistGate.o: /usr/include/mach/machine/vm_types.h
PointlistGate.o: /usr/include/sys/select.h /usr/include/getopt.h
PointlistGate.o: /usr/include/sys/limits.h
PointlistGate.o: /usr/include/sys/machine/machlimits.h
PointlistGate.o: /usr/include/sys/syslimits.h
PointlistGate.o: /usr/include/sys/machine/machtime.h
PointlistGate.o: /usr/include/sys/rt_limits.h
PointlistIterator.o: PointlistIterator.h
SingleItemIterator.o: SingleItemIterator.h
TrueGate.o: TrueGate.h Gate.h ConstituentIterator.h ActualIterator.h
TrueGate.o: SingleItemIterator.h
