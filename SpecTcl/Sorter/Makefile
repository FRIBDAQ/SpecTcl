#
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
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr

OBJECTS=DictionaryException.o  Histogrammer.o Spectrum.o Spectrum1DL.o \
	Spectrum2DW.o Spectrum1DW.o Spectrum2DB.o \
	BitSpectrumW.o BitSpectrumL.o SummarySpectrumW.o SummarySpectrumB.o \
	SpectrumFactory.o SpectrumFactoryException.o SnapshotSpectrum.o \
	Gamma1DW.o Gamma1DL.o Gamma2DW.o Gamma2DB.o GateMediator.o

system: libSorting.a
	$(ECHO) Library rebuild complete.

clean: 
	$(RM) $(OBJECTS)
	$(RM) libSorting.a

install:
	$(INSTALL) libSorting.a $(INSTDIR)/Lib
	$(INSTALL) *.h          $(INSTDIR)/Include

depend:
	$(DEPEND) $(INCLUDEDIRS) *.cpp

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
# DO NOT DELETE

AnalysisTest.o: Histogrammer.h Dictionary.h Parameter.h /usr/include/limits.h
AnalysisTest.o: /usr/include/standards.h /usr/include/machine/machlimits.h
AnalysisTest.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
AnalysisTest.o: /usr/include/sys/rt_limits.h Spectrum.h Spectrum1DL.h
AnalysisTest.o: Spectrum2DW.h Spectrum2DB.h Spectrum1DW.h Gamma1DW.h
AnalysisTest.o: Gamma1DL.h Gamma2DW.h Gamma2DB.h
BitSpectrumL.o: BitSpectrumL.h Spectrum.h Parameter.h /usr/include/limits.h
BitSpectrumL.o: /usr/include/standards.h /usr/include/machine/machlimits.h
BitSpectrumL.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
BitSpectrumL.o: /usr/include/sys/rt_limits.h /usr/include/assert.h
BitSpectrumL.o: /usr/include/math.h /usr/include/stdlib.h
BitSpectrumL.o: /usr/include/getopt.h /usr/include/sys/types.h
BitSpectrumL.o: /usr/include/mach/machine/vm_types.h
BitSpectrumL.o: /usr/include/sys/select.h
BitSpectrumW.o: BitSpectrumW.h Spectrum.h Parameter.h /usr/include/limits.h
BitSpectrumW.o: /usr/include/standards.h /usr/include/machine/machlimits.h
BitSpectrumW.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
BitSpectrumW.o: /usr/include/sys/rt_limits.h /usr/include/assert.h
BitSpectrumW.o: /usr/include/math.h /usr/include/stdlib.h
BitSpectrumW.o: /usr/include/getopt.h /usr/include/sys/types.h
BitSpectrumW.o: /usr/include/mach/machine/vm_types.h
BitSpectrumW.o: /usr/include/sys/select.h
DictionaryException.o: DictionaryException.h /usr/include/stdio.h
DictionaryException.o: /usr/include/standards.h /usr/include/sys/seek.h
DictionaryException.o: /usr/include/va_list.h /usr/include/sys/types.h
DictionaryException.o: /usr/include/mach/machine/vm_types.h
DictionaryException.o: /usr/include/sys/select.h /usr/include/getopt.h
DictionaryException.o: /usr/include/sys/limits.h
DictionaryException.o: /usr/include/machine/machlimits.h
DictionaryException.o: /usr/include/sys/syslimits.h
DictionaryException.o: /usr/include/machine/machtime.h
DictionaryException.o: /usr/include/sys/rt_limits.h /usr/include/assert.h
DictionaryException.o: /usr/include/string.h /usr/include/strings.h
Gamma1DL.o: Gamma1DL.h Parameter.h /usr/include/limits.h
Gamma1DL.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Gamma1DL.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Gamma1DL.o: /usr/include/sys/rt_limits.h
Gamma1DW.o: Gamma1DW.h Parameter.h /usr/include/limits.h
Gamma1DW.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Gamma1DW.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Gamma1DW.o: /usr/include/sys/rt_limits.h
Gamma2DB.o: Gamma2DB.h Parameter.h /usr/include/limits.h
Gamma2DB.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Gamma2DB.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Gamma2DB.o: /usr/include/sys/rt_limits.h
Gamma2DW.o: Gamma2DW.h Parameter.h /usr/include/limits.h
Gamma2DW.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Gamma2DW.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Gamma2DW.o: /usr/include/sys/rt_limits.h
GateMediator.o: GateMediator.h Spectrum.h
Histogrammer.o: Histogrammer.h Dictionary.h Parameter.h /usr/include/limits.h
Histogrammer.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Histogrammer.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Histogrammer.o: /usr/include/sys/rt_limits.h Spectrum.h DictionaryException.h
Histogrammer.o: /usr/include/assert.h GateMediator.h
SnapshotSpectrum.o: SnapshotSpectrum.h Spectrum.h
Spectrum.o: Spectrum.h /usr/include/string.h /usr/include/standards.h
Spectrum.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
Spectrum.o: /usr/include/sys/select.h /usr/include/strings.h
Spectrum1DL.o: Spectrum1DL.h Parameter.h /usr/include/limits.h
Spectrum1DL.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Spectrum1DL.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Spectrum1DL.o: /usr/include/sys/rt_limits.h
Spectrum1DW.o: Spectrum1DW.h Parameter.h /usr/include/limits.h
Spectrum1DW.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Spectrum1DW.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Spectrum1DW.o: /usr/include/sys/rt_limits.h
Spectrum2DB.o: Spectrum2DB.h Parameter.h /usr/include/limits.h
Spectrum2DB.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Spectrum2DB.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Spectrum2DB.o: /usr/include/sys/rt_limits.h
Spectrum2DW.o: Spectrum2DW.h Parameter.h /usr/include/limits.h
Spectrum2DW.o: /usr/include/standards.h /usr/include/machine/machlimits.h
Spectrum2DW.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
Spectrum2DW.o: /usr/include/sys/rt_limits.h
SpectrumFactory.o: SpectrumFactory.h Parameter.h /usr/include/limits.h
SpectrumFactory.o: /usr/include/standards.h /usr/include/machine/machlimits.h
SpectrumFactory.o: /usr/include/sys/syslimits.h
SpectrumFactory.o: /usr/include/machine/machtime.h
SpectrumFactory.o: /usr/include/sys/rt_limits.h SpectrumFactoryException.h
SpectrumFactory.o: /usr/include/string.h /usr/include/sys/types.h
SpectrumFactory.o: /usr/include/mach/machine/vm_types.h
SpectrumFactory.o: /usr/include/sys/select.h /usr/include/strings.h
SpectrumFactory.o: DictionaryException.h Spectrum.h Spectrum1DL.h
SpectrumFactory.o: Spectrum1DW.h Spectrum2DW.h Spectrum2DB.h BitSpectrumL.h
SpectrumFactory.o: BitSpectrumW.h SummarySpectrumW.h SummarySpectrumB.h
SpectrumFactory.o: Gamma1DW.h Gamma1DL.h Gamma2DB.h Gamma2DW.h Histogrammer.h
SpectrumFactory.o: Dictionary.h
SpectrumFactoryException.o: SpectrumFactoryException.h /usr/include/string.h
SpectrumFactoryException.o: /usr/include/standards.h /usr/include/sys/types.h
SpectrumFactoryException.o: /usr/include/mach/machine/vm_types.h
SpectrumFactoryException.o: /usr/include/sys/select.h /usr/include/strings.h
SpectrumFactoryException.o: /usr/include/stdio.h /usr/include/sys/seek.h
SpectrumFactoryException.o: /usr/include/va_list.h /usr/include/getopt.h
SpectrumFactoryException.o: /usr/include/sys/limits.h
SpectrumFactoryException.o: /usr/include/machine/machlimits.h
SpectrumFactoryException.o: /usr/include/sys/syslimits.h
SpectrumFactoryException.o: /usr/include/machine/machtime.h
SpectrumFactoryException.o: /usr/include/sys/rt_limits.h
SummarySpectrumB.o: SummarySpectrumB.h Parameter.h /usr/include/limits.h
SummarySpectrumB.o: /usr/include/standards.h
SummarySpectrumB.o: /usr/include/machine/machlimits.h
SummarySpectrumB.o: /usr/include/sys/syslimits.h
SummarySpectrumB.o: /usr/include/machine/machtime.h
SummarySpectrumB.o: /usr/include/sys/rt_limits.h /usr/include/assert.h
SummarySpectrumW.o: SummarySpectrumW.h Parameter.h /usr/include/limits.h
SummarySpectrumW.o: /usr/include/standards.h
SummarySpectrumW.o: /usr/include/machine/machlimits.h
SummarySpectrumW.o: /usr/include/sys/syslimits.h
SummarySpectrumW.o: /usr/include/machine/machtime.h
SummarySpectrumW.o: /usr/include/sys/rt_limits.h /usr/include/assert.h
