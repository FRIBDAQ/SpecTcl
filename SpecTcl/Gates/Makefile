#
#  Makefile for the gates object library (SpecTcl facility).
#


SPECTCL_VERSION = -DVERSION=$(VERSION)

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
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"' $(SPECTCL_VERSION)
LINK=$($(OS)LD) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS) $(SPECTCL_VERSION)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
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
	$(DEPEND) -Y. *.cpp

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
