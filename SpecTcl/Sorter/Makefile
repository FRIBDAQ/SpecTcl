
#  Makefile for the sorting part of the Histogramming program.
#  targets include:
#     system(default)             - Builds the software.
#     clean                       - Kills off .o, .a files.
#     depend                      - Rebuilds the dependencies with makedepend.
#     install                     - Install the software under $(INSTDIR)
#     tests                       - Rebuilds test program(s) against sofware
#                                   in current directory.
#
# Change Log:
#   June 29, 1999 - Added the following Objects:
#                    Spectrum1DW.o      - 1 d word spectra.
#                    Spectrum2DB.o      - 2 d byte spectra.
#                    BitSpectrumW.o     - Word spectrum of bit masks.
#                    BitSpectrumL.o     - Byte spectrum of bitmasks.
#                    SummarySpectrumW.o - Word summary spectrum.
#                    SummarySpectrumB.o - Byte summary spectrum.
#   June 29, 2001 - Added the following Objects:
#                    Gamma1DW.o         - 1 d gamma word spectra
#                    Gamma1DL.o         - 1 d gamma longword spectra
#                    Gamma2DB.o         - 2 d gamma byte spectra
#                    Gamma2DW.o         - 2 d gamma word spectra
# $Log$
# Revision 4.26  2003/06/19 19:03:09  ron-fox
# Usual inconsequential changes as a result of make depend.
#
# Revision 4.25  2003/04/01 19:53:43  ron-fox
# Support for Real valued parameters and spectra with arbitrary binnings.
#

SPECTCL_VERSION = -DVERSION=$(VERSION)
CYGWININCS=-I/usr/local/tcl/include
CYGWINLIBDIR=-L/usr/local/tcl/lib

INCLUDEDIRS= $($(OS)INCS) -I. -I$(INSTDIR)/Include -I/usr/local/include \
	-I/usr/local/daq/include -I/usr/include/g++-3

LIBDIRS= $($(OS)LIBDIRS) -L. -L../Lib -L/usr/local \
	-L/usr/local/daq/lib -L/usr/X11/lib \
	-L$(INSTDIR)/Lib

LIBS=   -lSorting -lXplus -lGates -lAnalysis -lEventSource \
	-ltclPlus -lException \
	-lXamine \
	-ltk -ltcl -lXt -lX11

ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) $(SPECTCL_VERSION)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) $(SPECTCL_VERSION)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
ARCHIVE=ar cr

OBJECTS=DictionaryException.o  Histogrammer.o Spectrum.o Spectrum1DL.o \
	Spectrum2DW.o Spectrum1DW.o Spectrum2DB.o \
	BitSpectrumW.o BitSpectrumL.o SummarySpectrumW.o SummarySpectrumB.o \
	SpectrumFactory.o SpectrumFactoryException.o SnapshotSpectrum.o \
	Gamma1DW.o Gamma1DL.o Gamma2DW.o Gamma2DB.o GateMediator.o \
	CAxis.o  \
	CParameterMapping.o Parameter.o

#	MSpectrum1DW.o MSpectrum1DL.o MSpectrum2DB.o MSpectrum2DW.o \
#	MGamma1DW.o MGamma1DL.o MGamma2DB.o MGamma2DW.o 

.cpp.o:
	$(COMPILE) $<

system: libSorting.a
	$(ECHO) Library rebuild complete.

clean: 
	$(RM) $(OBJECTS)
	$(RM) libSorting.a

install:
	$(INSTALL) libSorting.a $(INSTDIR)/Lib
	$(INSTALL) *.h          $(INSTDIR)/Include

depend:
	$(DEPEND) -Y.  *.cpp

tests: AnalysisTest
	$(ECHO) Test 'program(s)' are rebuilt.

libSorting.a: $(OBJECTS)
	$(ARCHIVE) libSorting.a $(OBJECTS)

DictionaryException.o:  DictionaryException.cpp
	$(COMPILE) DictionaryException.cpp

AnalysisTest: AnalysisTest.cpp system
	$(LINK) -o AnalysisTest AnalysisTest.cpp $(LIBS)

Histogrammer.o: Histogrammer.cpp
	$(COMPILE)  Histogrammer.cpp

Spectrum.o: Spectrum.cpp
	$(COMPILE) Spectrum.cpp

Spectrum1DL.o: Spectrum1DL.cpp
	$(COMPILE) Spectrum1DL.cpp

Spectrum2DW.o: Spectrum2DW.cpp
	$(COMPILE) Spectrum2DW.cpp

Spectrum1DW.o: Spectrum1DW.cpp
	$(COMPILE) Spectrum1DW.cpp

Spectrum2DB.o: Spectrum2DB.cpp
	$(COMPILE) Spectrum2DB.cpp

BitSpectrumW.o: BitSpectrumW.cpp
	$(COMPILE) BitSpectrumW.cpp

BitSpectrumL.o: BitSpectrumL.cpp
	$(COMPILE) BitSpectrumL.cpp

SummarySpectrumW.o: SummarySpectrumW.cpp
	$(COMPILE) SummarySpectrumW.cpp

SummarySpectrumB.o: SummarySpectrumB.cpp
	$(COMPILE) SummarySpectrumB.cpp

SpectrumFactory.o: SpectrumFactory.cpp
	$(COMPILE) SpectrumFactory.cpp

SpectrumFactoryException.o: SpectrumFactoryException.cpp
	$(COMPILE) SpectrumFactoryException.cpp

SnapshotSpectrum.o: SnapshotSpectrum.cpp
	$(COMPILE) SnapshotSpectrum.cpp

Gamma1DW.o: Gamma1DW.cpp
	$(COMPILE) Gamma1DW.cpp

Gamma1DL.o: Gamma1DL.cpp
	$(COMPILE) Gamma1DL.cpp

Gamma2DW.o: Gamma2DW.cpp
	$(COMPILE) Gamma2DW.cpp

Gamma2DB.o: Gamma2DB.cpp
	$(COMPILE) Gamma2DB.cpp

GateMediator.o: GateMediator.cpp
	$(COMPILE) GateMediator.cpp

#MSpectrum1DW.o: MSpectrum1DW.cpp
#	$(COMPILE) MSpectrum1DW.cpp

#MSpectrum1DL.o: MSpectrum1DL.cpp
#	$(COMPILE) MSpectrum1DL.cpp

#MSpectrum2DB.o: MSpectrum2DB.cpp
#	$(COMPILE) MSpectrum2DB.cpp

#MSpectrum2DW.o: MSpectrum2DW.cpp
#	$(COMPILE) MSpectrum2DW.cpp

#MGamma1DL.o: MGamma1DL.cpp
#	$(COMPILE) MGamma1DL.cpp

#MGamma1DW.o: MGamma1DW.cpp
#	$(COMPILE) MGamma1DW.cpp

#MGamma2DB.o: MGamma2DB.cpp
#	$(COMPILE) MGamma2DB.cpp

#MGamma2DW.o: MGamma2DW.cpp
#	$(COMPILE) MGamma2DW.cpp
# DO NOT DELETE

AnalysisTest.o: Histogrammer.h Dictionary.h Parameter.h Spectrum.h CAxis.h
AnalysisTest.o: CParameterMapping.h Spectrum1DL.h Spectrum2DW.h Spectrum2DB.h
AnalysisTest.o: Spectrum1DW.h Gamma1DW.h Gamma1DL.h Gamma2DW.h Gamma2DB.h
BitSpectrumL.o: BitSpectrumL.h Spectrum.h CAxis.h CParameterMapping.h
BitSpectrumL.o: Parameter.h
BitSpectrumW.o: BitSpectrumW.h Spectrum.h CAxis.h CParameterMapping.h
BitSpectrumW.o: Parameter.h
CAxis.o: CAxis.h CParameterMapping.h
CAxisTests.o: CAxisTests.h CAxis.h CParameterMapping.h
CParameterMapping.o: CParameterMapping.h Parameter.h
CParameterMappingTest.o: CParameterMappingTest.h CParameterMapping.h
DictionaryException.o: DictionaryException.h
Gamma1DL.o: Gamma1DL.h Parameter.h CParameterMapping.h CAxis.h
Gamma1DW.o: Gamma1DW.h Parameter.h CParameterMapping.h CAxis.h
Gamma2DB.o: Gamma2DB.h Parameter.h
Gamma2DW.o: Gamma2DW.h Parameter.h
GateMediator.o: GateMediator.h Spectrum.h CAxis.h CParameterMapping.h
Histogrammer.o: Histogrammer.h Dictionary.h Parameter.h Spectrum.h CAxis.h
Histogrammer.o: CParameterMapping.h DictionaryException.h GateMediator.h
Histogrammer.o: Gamma2DW.h
MGamma1DL.o: MGamma1DL.h Gamma1DL.h Parameter.h
MGamma1DW.o: MGamma1DW.h Gamma1DW.h Parameter.h
MGamma2DB.o: MGamma2DB.h Gamma2DB.h Parameter.h
MGamma2DW.o: MGamma2DW.h Gamma2DW.h Parameter.h
MSpectrum1DL.o: MSpectrum1DL.h Spectrum1DL.h Parameter.h
MSpectrum1DW.o: MSpectrum1DW.h Spectrum1DW.h Parameter.h
MSpectrum2DB.o: MSpectrum2DB.h Spectrum2DB.h Parameter.h
MSpectrum2DW.o: MSpectrum2DW.h Spectrum2DW.h Parameter.h
Parameter.o: Parameter.h
ScalingTests.o: CParameterMappingTest.h CParameterMapping.h CAxisTests.h
ScalingTests.o: CAxis.h
SnapshotSpectrum.o: SnapshotSpectrum.h Spectrum.h CAxis.h CParameterMapping.h
Spectrum.o: Spectrum.h CAxis.h CParameterMapping.h
Spectrum1DL.o: Spectrum1DL.h Parameter.h CAxis.h CParameterMapping.h
Spectrum1DW.o: Spectrum1DW.h Parameter.h CAxis.h CParameterMapping.h
Spectrum2DB.o: Spectrum2DB.h Parameter.h CAxis.h CParameterMapping.h
Spectrum2DW.o: Spectrum2DW.h Parameter.h CAxis.h CParameterMapping.h
SpectrumFactory.o: SpectrumFactory.h Parameter.h SpectrumFactoryException.h
SpectrumFactory.o: DictionaryException.h Spectrum.h CAxis.h
SpectrumFactory.o: CParameterMapping.h Spectrum1DL.h Spectrum1DW.h
SpectrumFactory.o: Spectrum2DW.h Spectrum2DB.h BitSpectrumL.h BitSpectrumW.h
SpectrumFactory.o: SummarySpectrumW.h SummarySpectrumB.h Gamma1DW.h
SpectrumFactory.o: Gamma1DL.h Gamma2DB.h Gamma2DW.h Histogrammer.h
SpectrumFactory.o: Dictionary.h
SpectrumFactoryException.o: SpectrumFactoryException.h
SummarySpectrumB.o: SummarySpectrumB.h Parameter.h
SummarySpectrumW.o: SummarySpectrumW.h Parameter.h
