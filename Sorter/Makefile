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

AnalysisTest.o: Histogrammer.h Dictionary.h /usr/include/g++-3/map
AnalysisTest.o: /usr/include/g++-3/stl_tree.h
AnalysisTest.o: /usr/include/g++-3/stl_algobase.h
AnalysisTest.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
AnalysisTest.o: /usr/include/bits/types.h /usr/include/features.h
AnalysisTest.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
AnalysisTest.o: /usr/include/wchar.h /usr/include/bits/wchar.h
AnalysisTest.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
AnalysisTest.o: /usr/include/g++-3/stl_pair.h
AnalysisTest.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
AnalysisTest.o: /usr/include/limits.h
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
AnalysisTest.o: /usr/include/stdlib.h
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
AnalysisTest.o: /usr/include/g++-3/iostream.h /usr/include/g++-3/streambuf.h
AnalysisTest.o: /usr/include/libio.h
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
AnalysisTest.o: /usr/include/g++-3/stl_iterator.h
AnalysisTest.o: /usr/include/g++-3/stl_alloc.h /usr/include/assert.h
AnalysisTest.o: /usr/include/g++-3/stl_construct.h
AnalysisTest.o: /usr/include/g++-3/stl_function.h
AnalysisTest.o: /usr/include/g++-3/stl_map.h
AnalysisTest.o: /usr/include/g++-3/stl_multimap.h
AnalysisTest.o: /usr/include/g++-3/algorithm
AnalysisTest.o: /usr/include/g++-3/stl_uninitialized.h
AnalysisTest.o: /usr/include/g++-3/stl_tempbuf.h
AnalysisTest.o: /usr/include/g++-3/stl_algo.h /usr/include/g++-3/stl_heap.h
AnalysisTest.o: /usr/include/g++-3/string /usr/include/g++-3/std/bastring.h
AnalysisTest.o: /usr/include/g++-3/cstddef /usr/include/g++-3/std/straits.h
AnalysisTest.o: /usr/include/g++-3/cctype /usr/include/ctype.h
AnalysisTest.o: /usr/include/endian.h /usr/include/bits/endian.h
AnalysisTest.o: /usr/include/g++-3/cstring /usr/include/g++-3/alloc.h
AnalysisTest.o: /usr/include/g++-3/iterator /usr/include/g++-3/cassert
AnalysisTest.o: /usr/include/g++-3/std/bastring.cc Parameter.h Spectrum.h
AnalysisTest.o: /usr/include/g++-3/vector /usr/include/g++-3/stl_vector.h
AnalysisTest.o: /usr/include/g++-3/stl_bvector.h Spectrum1DL.h Spectrum2DW.h
AnalysisTest.o: Spectrum2DB.h Spectrum1DW.h Gamma1DW.h Gamma1DL.h Gamma2DW.h
AnalysisTest.o: Gamma2DB.h
BitSpectrumL.o: BitSpectrumL.h Spectrum.h /usr/include/g++-3/vector
BitSpectrumL.o: /usr/include/g++-3/stl_algobase.h
BitSpectrumL.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
BitSpectrumL.o: /usr/include/bits/types.h /usr/include/features.h
BitSpectrumL.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
BitSpectrumL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
BitSpectrumL.o: /usr/include/wchar.h /usr/include/bits/wchar.h
BitSpectrumL.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
BitSpectrumL.o: /usr/include/g++-3/stl_pair.h
BitSpectrumL.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
BitSpectrumL.o: /usr/include/limits.h
BitSpectrumL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
BitSpectrumL.o: /usr/include/stdlib.h
BitSpectrumL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
BitSpectrumL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
BitSpectrumL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
BitSpectrumL.o: /usr/include/g++-3/iostream.h /usr/include/g++-3/streambuf.h
BitSpectrumL.o: /usr/include/libio.h
BitSpectrumL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
BitSpectrumL.o: /usr/include/g++-3/stl_iterator.h
BitSpectrumL.o: /usr/include/g++-3/stl_alloc.h /usr/include/assert.h
BitSpectrumL.o: /usr/include/g++-3/stl_construct.h
BitSpectrumL.o: /usr/include/g++-3/stl_uninitialized.h
BitSpectrumL.o: /usr/include/g++-3/stl_vector.h
BitSpectrumL.o: /usr/include/g++-3/stl_bvector.h /usr/include/g++-3/string
BitSpectrumL.o: /usr/include/g++-3/std/bastring.h /usr/include/g++-3/cstddef
BitSpectrumL.o: /usr/include/g++-3/std/straits.h /usr/include/g++-3/cctype
BitSpectrumL.o: /usr/include/ctype.h /usr/include/endian.h
BitSpectrumL.o: /usr/include/bits/endian.h /usr/include/g++-3/cstring
BitSpectrumL.o: /usr/include/g++-3/alloc.h /usr/include/g++-3/iterator
BitSpectrumL.o: /usr/include/g++-3/cassert /usr/include/g++-3/std/bastring.cc
BitSpectrumL.o: Parameter.h /usr/include/math.h /usr/include/bits/huge_val.h
BitSpectrumL.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
BitSpectrumW.o: BitSpectrumW.h Spectrum.h /usr/include/g++-3/vector
BitSpectrumW.o: /usr/include/g++-3/stl_algobase.h
BitSpectrumW.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
BitSpectrumW.o: /usr/include/bits/types.h /usr/include/features.h
BitSpectrumW.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
BitSpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
BitSpectrumW.o: /usr/include/wchar.h /usr/include/bits/wchar.h
BitSpectrumW.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
BitSpectrumW.o: /usr/include/g++-3/stl_pair.h
BitSpectrumW.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
BitSpectrumW.o: /usr/include/limits.h
BitSpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
BitSpectrumW.o: /usr/include/stdlib.h
BitSpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
BitSpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
BitSpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
BitSpectrumW.o: /usr/include/g++-3/iostream.h /usr/include/g++-3/streambuf.h
BitSpectrumW.o: /usr/include/libio.h
BitSpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
BitSpectrumW.o: /usr/include/g++-3/stl_iterator.h
BitSpectrumW.o: /usr/include/g++-3/stl_alloc.h /usr/include/assert.h
BitSpectrumW.o: /usr/include/g++-3/stl_construct.h
BitSpectrumW.o: /usr/include/g++-3/stl_uninitialized.h
BitSpectrumW.o: /usr/include/g++-3/stl_vector.h
BitSpectrumW.o: /usr/include/g++-3/stl_bvector.h /usr/include/g++-3/string
BitSpectrumW.o: /usr/include/g++-3/std/bastring.h /usr/include/g++-3/cstddef
BitSpectrumW.o: /usr/include/g++-3/std/straits.h /usr/include/g++-3/cctype
BitSpectrumW.o: /usr/include/ctype.h /usr/include/endian.h
BitSpectrumW.o: /usr/include/bits/endian.h /usr/include/g++-3/cstring
BitSpectrumW.o: /usr/include/g++-3/alloc.h /usr/include/g++-3/iterator
BitSpectrumW.o: /usr/include/g++-3/cassert /usr/include/g++-3/std/bastring.cc
BitSpectrumW.o: Parameter.h /usr/include/math.h /usr/include/bits/huge_val.h
BitSpectrumW.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
DictionaryException.o: DictionaryException.h /usr/include/g++-3/string
DictionaryException.o: /usr/include/g++-3/std/bastring.h
DictionaryException.o: /usr/include/g++-3/cstddef
DictionaryException.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
DictionaryException.o: /usr/include/g++-3/std/straits.h
DictionaryException.o: /usr/include/g++-3/cctype /usr/include/ctype.h
DictionaryException.o: /usr/include/features.h /usr/include/sys/cdefs.h
DictionaryException.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
DictionaryException.o: /usr/include/endian.h /usr/include/bits/endian.h
DictionaryException.o: /usr/include/g++-3/cstring /usr/include/string.h
DictionaryException.o: /usr/include/g++-3/alloc.h /usr/include/g++-3/iterator
DictionaryException.o: /usr/include/g++-3/stl_config.h
DictionaryException.o: /usr/include/_G_config.h /usr/include/wchar.h
DictionaryException.o: /usr/include/bits/wchar.h /usr/include/gconv.h
DictionaryException.o: /usr/include/g++-3/stl_relops.h
DictionaryException.o: /usr/include/g++-3/iostream.h
DictionaryException.o: /usr/include/g++-3/streambuf.h /usr/include/libio.h
DictionaryException.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
DictionaryException.o: /usr/include/g++-3/stl_iterator.h
DictionaryException.o: /usr/include/g++-3/cassert /usr/include/assert.h
DictionaryException.o: /usr/include/g++-3/std/bastring.cc
DictionaryException.o: /usr/include/stdio.h /usr/include/bits/stdio_lim.h
Gamma1DL.o: Gamma1DL.h Parameter.h /usr/include/limits.h
Gamma1DL.o: /usr/include/features.h /usr/include/sys/cdefs.h
Gamma1DL.o: /usr/include/gnu/stubs.h
Gamma1DL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Gamma1DW.o: Gamma1DW.h Parameter.h /usr/include/limits.h
Gamma1DW.o: /usr/include/features.h /usr/include/sys/cdefs.h
Gamma1DW.o: /usr/include/gnu/stubs.h
Gamma1DW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Gamma2DB.o: Gamma2DB.h Parameter.h /usr/include/limits.h
Gamma2DB.o: /usr/include/features.h /usr/include/sys/cdefs.h
Gamma2DB.o: /usr/include/gnu/stubs.h
Gamma2DB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Gamma2DW.o: Gamma2DW.h Parameter.h /usr/include/limits.h
Gamma2DW.o: /usr/include/features.h /usr/include/sys/cdefs.h
Gamma2DW.o: /usr/include/gnu/stubs.h
Gamma2DW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
GateMediator.o: GateMediator.h Spectrum.h /usr/include/g++-3/vector
GateMediator.o: /usr/include/g++-3/stl_algobase.h
GateMediator.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
GateMediator.o: /usr/include/bits/types.h /usr/include/features.h
GateMediator.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GateMediator.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
GateMediator.o: /usr/include/wchar.h /usr/include/bits/wchar.h
GateMediator.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
GateMediator.o: /usr/include/g++-3/stl_pair.h
GateMediator.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
GateMediator.o: /usr/include/limits.h
GateMediator.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
GateMediator.o: /usr/include/stdlib.h
GateMediator.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
GateMediator.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
GateMediator.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
GateMediator.o: /usr/include/g++-3/iostream.h /usr/include/g++-3/streambuf.h
GateMediator.o: /usr/include/libio.h
GateMediator.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
GateMediator.o: /usr/include/g++-3/stl_iterator.h
GateMediator.o: /usr/include/g++-3/stl_alloc.h /usr/include/assert.h
GateMediator.o: /usr/include/g++-3/stl_construct.h
GateMediator.o: /usr/include/g++-3/stl_uninitialized.h
GateMediator.o: /usr/include/g++-3/stl_vector.h
GateMediator.o: /usr/include/g++-3/stl_bvector.h /usr/include/g++-3/string
GateMediator.o: /usr/include/g++-3/std/bastring.h /usr/include/g++-3/cstddef
GateMediator.o: /usr/include/g++-3/std/straits.h /usr/include/g++-3/cctype
GateMediator.o: /usr/include/ctype.h /usr/include/endian.h
GateMediator.o: /usr/include/bits/endian.h /usr/include/g++-3/cstring
GateMediator.o: /usr/include/g++-3/alloc.h /usr/include/g++-3/iterator
GateMediator.o: /usr/include/g++-3/cassert /usr/include/g++-3/std/bastring.cc
Histogrammer.o: Histogrammer.h Dictionary.h /usr/include/g++-3/map
Histogrammer.o: /usr/include/g++-3/stl_tree.h
Histogrammer.o: /usr/include/g++-3/stl_algobase.h
Histogrammer.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
Histogrammer.o: /usr/include/bits/types.h /usr/include/features.h
Histogrammer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Histogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
Histogrammer.o: /usr/include/wchar.h /usr/include/bits/wchar.h
Histogrammer.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
Histogrammer.o: /usr/include/g++-3/stl_pair.h
Histogrammer.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
Histogrammer.o: /usr/include/limits.h
Histogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Histogrammer.o: /usr/include/stdlib.h
Histogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
Histogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
Histogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
Histogrammer.o: /usr/include/g++-3/iostream.h /usr/include/g++-3/streambuf.h
Histogrammer.o: /usr/include/libio.h
Histogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
Histogrammer.o: /usr/include/g++-3/stl_iterator.h
Histogrammer.o: /usr/include/g++-3/stl_alloc.h /usr/include/assert.h
Histogrammer.o: /usr/include/g++-3/stl_construct.h
Histogrammer.o: /usr/include/g++-3/stl_function.h
Histogrammer.o: /usr/include/g++-3/stl_map.h
Histogrammer.o: /usr/include/g++-3/stl_multimap.h
Histogrammer.o: /usr/include/g++-3/algorithm
Histogrammer.o: /usr/include/g++-3/stl_uninitialized.h
Histogrammer.o: /usr/include/g++-3/stl_tempbuf.h
Histogrammer.o: /usr/include/g++-3/stl_algo.h /usr/include/g++-3/stl_heap.h
Histogrammer.o: /usr/include/g++-3/string /usr/include/g++-3/std/bastring.h
Histogrammer.o: /usr/include/g++-3/cstddef /usr/include/g++-3/std/straits.h
Histogrammer.o: /usr/include/g++-3/cctype /usr/include/ctype.h
Histogrammer.o: /usr/include/endian.h /usr/include/bits/endian.h
Histogrammer.o: /usr/include/g++-3/cstring /usr/include/g++-3/alloc.h
Histogrammer.o: /usr/include/g++-3/iterator /usr/include/g++-3/cassert
Histogrammer.o: /usr/include/g++-3/std/bastring.cc Parameter.h Spectrum.h
Histogrammer.o: /usr/include/g++-3/vector /usr/include/g++-3/stl_vector.h
Histogrammer.o: /usr/include/g++-3/stl_bvector.h DictionaryException.h
Histogrammer.o: GateMediator.h
SnapshotSpectrum.o: SnapshotSpectrum.h Spectrum.h /usr/include/g++-3/vector
SnapshotSpectrum.o: /usr/include/g++-3/stl_algobase.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
SnapshotSpectrum.o: /usr/include/bits/types.h /usr/include/features.h
SnapshotSpectrum.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
SnapshotSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
SnapshotSpectrum.o: /usr/include/wchar.h /usr/include/bits/wchar.h
SnapshotSpectrum.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_pair.h
SnapshotSpectrum.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
SnapshotSpectrum.o: /usr/include/limits.h
SnapshotSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
SnapshotSpectrum.o: /usr/include/stdlib.h
SnapshotSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
SnapshotSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
SnapshotSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
SnapshotSpectrum.o: /usr/include/g++-3/iostream.h
SnapshotSpectrum.o: /usr/include/g++-3/streambuf.h /usr/include/libio.h
SnapshotSpectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_iterator.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_alloc.h /usr/include/assert.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_construct.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_uninitialized.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_vector.h
SnapshotSpectrum.o: /usr/include/g++-3/stl_bvector.h
Spectrum.o: Spectrum.h /usr/include/g++-3/vector
Spectrum.o: /usr/include/g++-3/stl_algobase.h /usr/include/g++-3/stl_config.h
Spectrum.o: /usr/include/_G_config.h /usr/include/bits/types.h
Spectrum.o: /usr/include/features.h /usr/include/sys/cdefs.h
Spectrum.o: /usr/include/gnu/stubs.h
Spectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
Spectrum.o: /usr/include/wchar.h /usr/include/bits/wchar.h
Spectrum.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
Spectrum.o: /usr/include/g++-3/stl_pair.h /usr/include/g++-3/type_traits.h
Spectrum.o: /usr/include/string.h /usr/include/limits.h
Spectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Spectrum.o: /usr/include/stdlib.h
Spectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
Spectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
Spectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
Spectrum.o: /usr/include/g++-3/iostream.h /usr/include/g++-3/streambuf.h
Spectrum.o: /usr/include/libio.h
Spectrum.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
Spectrum.o: /usr/include/g++-3/stl_iterator.h /usr/include/g++-3/stl_alloc.h
Spectrum.o: /usr/include/assert.h /usr/include/g++-3/stl_construct.h
Spectrum.o: /usr/include/g++-3/stl_uninitialized.h
Spectrum.o: /usr/include/g++-3/stl_vector.h /usr/include/g++-3/stl_bvector.h
Spectrum1DL.o: Spectrum1DL.h Parameter.h /usr/include/limits.h
Spectrum1DL.o: /usr/include/features.h /usr/include/sys/cdefs.h
Spectrum1DL.o: /usr/include/gnu/stubs.h
Spectrum1DL.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Spectrum1DW.o: Spectrum1DW.h Parameter.h /usr/include/limits.h
Spectrum1DW.o: /usr/include/features.h /usr/include/sys/cdefs.h
Spectrum1DW.o: /usr/include/gnu/stubs.h
Spectrum1DW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Spectrum2DB.o: Spectrum2DB.h Parameter.h /usr/include/limits.h
Spectrum2DB.o: /usr/include/features.h /usr/include/sys/cdefs.h
Spectrum2DB.o: /usr/include/gnu/stubs.h
Spectrum2DB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
Spectrum2DW.o: Spectrum2DW.h Parameter.h /usr/include/limits.h
Spectrum2DW.o: /usr/include/features.h /usr/include/sys/cdefs.h
Spectrum2DW.o: /usr/include/gnu/stubs.h
Spectrum2DW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
SpectrumFactory.o: SpectrumFactory.h Parameter.h /usr/include/limits.h
SpectrumFactory.o: /usr/include/features.h /usr/include/sys/cdefs.h
SpectrumFactory.o: /usr/include/gnu/stubs.h
SpectrumFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
SpectrumFactory.o: /usr/include/g++-3/vector
SpectrumFactory.o: /usr/include/g++-3/stl_algobase.h
SpectrumFactory.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
SpectrumFactory.o: /usr/include/bits/types.h
SpectrumFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
SpectrumFactory.o: /usr/include/wchar.h /usr/include/bits/wchar.h
SpectrumFactory.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
SpectrumFactory.o: /usr/include/g++-3/stl_pair.h
SpectrumFactory.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
SpectrumFactory.o: /usr/include/stdlib.h
SpectrumFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
SpectrumFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
SpectrumFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
SpectrumFactory.o: /usr/include/g++-3/iostream.h
SpectrumFactory.o: /usr/include/g++-3/streambuf.h /usr/include/libio.h
SpectrumFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
SpectrumFactory.o: /usr/include/g++-3/stl_iterator.h
SpectrumFactory.o: /usr/include/g++-3/stl_alloc.h /usr/include/assert.h
SpectrumFactory.o: /usr/include/g++-3/stl_construct.h
SpectrumFactory.o: /usr/include/g++-3/stl_uninitialized.h
SpectrumFactory.o: /usr/include/g++-3/stl_vector.h
SpectrumFactory.o: /usr/include/g++-3/stl_bvector.h /usr/include/g++-3/string
SpectrumFactory.o: /usr/include/g++-3/std/bastring.h
SpectrumFactory.o: /usr/include/g++-3/cstddef
SpectrumFactory.o: /usr/include/g++-3/std/straits.h /usr/include/g++-3/cctype
SpectrumFactory.o: /usr/include/ctype.h /usr/include/endian.h
SpectrumFactory.o: /usr/include/bits/endian.h /usr/include/g++-3/cstring
SpectrumFactory.o: /usr/include/g++-3/alloc.h /usr/include/g++-3/iterator
SpectrumFactory.o: /usr/include/g++-3/cassert
SpectrumFactory.o: /usr/include/g++-3/std/bastring.cc
SpectrumFactory.o: SpectrumFactoryException.h DictionaryException.h
SpectrumFactory.o: Spectrum.h Spectrum1DL.h Spectrum1DW.h Spectrum2DW.h
SpectrumFactory.o: Spectrum2DB.h BitSpectrumL.h BitSpectrumW.h
SpectrumFactory.o: SummarySpectrumW.h SummarySpectrumB.h Gamma1DW.h
SpectrumFactory.o: Gamma1DL.h Gamma2DB.h Gamma2DW.h Histogrammer.h
SpectrumFactory.o: Dictionary.h /usr/include/g++-3/map
SpectrumFactory.o: /usr/include/g++-3/stl_tree.h
SpectrumFactory.o: /usr/include/g++-3/stl_function.h
SpectrumFactory.o: /usr/include/g++-3/stl_map.h
SpectrumFactory.o: /usr/include/g++-3/stl_multimap.h
SpectrumFactory.o: /usr/include/g++-3/algorithm
SpectrumFactory.o: /usr/include/g++-3/stl_tempbuf.h
SpectrumFactory.o: /usr/include/g++-3/stl_algo.h
SpectrumFactory.o: /usr/include/g++-3/stl_heap.h
SpectrumFactoryException.o: SpectrumFactoryException.h
SpectrumFactoryException.o: /usr/include/g++-3/string
SpectrumFactoryException.o: /usr/include/g++-3/std/bastring.h
SpectrumFactoryException.o: /usr/include/g++-3/cstddef
SpectrumFactoryException.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
SpectrumFactoryException.o: /usr/include/g++-3/std/straits.h
SpectrumFactoryException.o: /usr/include/g++-3/cctype /usr/include/ctype.h
SpectrumFactoryException.o: /usr/include/features.h /usr/include/sys/cdefs.h
SpectrumFactoryException.o: /usr/include/gnu/stubs.h
SpectrumFactoryException.o: /usr/include/bits/types.h /usr/include/endian.h
SpectrumFactoryException.o: /usr/include/bits/endian.h
SpectrumFactoryException.o: /usr/include/g++-3/cstring /usr/include/string.h
SpectrumFactoryException.o: /usr/include/g++-3/alloc.h
SpectrumFactoryException.o: /usr/include/g++-3/iterator
SpectrumFactoryException.o: /usr/include/g++-3/stl_config.h
SpectrumFactoryException.o: /usr/include/_G_config.h /usr/include/wchar.h
SpectrumFactoryException.o: /usr/include/bits/wchar.h /usr/include/gconv.h
SpectrumFactoryException.o: /usr/include/g++-3/stl_relops.h
SpectrumFactoryException.o: /usr/include/g++-3/iostream.h
SpectrumFactoryException.o: /usr/include/g++-3/streambuf.h
SpectrumFactoryException.o: /usr/include/libio.h
SpectrumFactoryException.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
SpectrumFactoryException.o: /usr/include/g++-3/stl_iterator.h
SpectrumFactoryException.o: /usr/include/g++-3/cassert /usr/include/assert.h
SpectrumFactoryException.o: /usr/include/g++-3/std/bastring.cc
SpectrumFactoryException.o: /usr/include/stdio.h
SpectrumFactoryException.o: /usr/include/bits/stdio_lim.h
SummarySpectrumB.o: SummarySpectrumB.h Parameter.h /usr/include/limits.h
SummarySpectrumB.o: /usr/include/features.h /usr/include/sys/cdefs.h
SummarySpectrumB.o: /usr/include/gnu/stubs.h
SummarySpectrumB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
SummarySpectrumB.o: /usr/include/g++-3/algorithm
SummarySpectrumB.o: /usr/include/g++-3/stl_algobase.h
SummarySpectrumB.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
SummarySpectrumB.o: /usr/include/bits/types.h
SummarySpectrumB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
SummarySpectrumB.o: /usr/include/wchar.h /usr/include/bits/wchar.h
SummarySpectrumB.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
SummarySpectrumB.o: /usr/include/g++-3/stl_pair.h
SummarySpectrumB.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
SummarySpectrumB.o: /usr/include/stdlib.h
SummarySpectrumB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
SummarySpectrumB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
SummarySpectrumB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
SummarySpectrumB.o: /usr/include/g++-3/iostream.h
SummarySpectrumB.o: /usr/include/g++-3/streambuf.h /usr/include/libio.h
SummarySpectrumB.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
SummarySpectrumB.o: /usr/include/g++-3/stl_iterator.h
SummarySpectrumB.o: /usr/include/g++-3/stl_construct.h
SummarySpectrumB.o: /usr/include/g++-3/stl_uninitialized.h
SummarySpectrumB.o: /usr/include/g++-3/stl_tempbuf.h
SummarySpectrumB.o: /usr/include/g++-3/stl_algo.h
SummarySpectrumB.o: /usr/include/g++-3/stl_heap.h /usr/include/assert.h
SummarySpectrumW.o: SummarySpectrumW.h Parameter.h /usr/include/limits.h
SummarySpectrumW.o: /usr/include/features.h /usr/include/sys/cdefs.h
SummarySpectrumW.o: /usr/include/gnu/stubs.h
SummarySpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
SummarySpectrumW.o: /usr/include/g++-3/algorithm
SummarySpectrumW.o: /usr/include/g++-3/stl_algobase.h
SummarySpectrumW.o: /usr/include/g++-3/stl_config.h /usr/include/_G_config.h
SummarySpectrumW.o: /usr/include/bits/types.h
SummarySpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
SummarySpectrumW.o: /usr/include/wchar.h /usr/include/bits/wchar.h
SummarySpectrumW.o: /usr/include/gconv.h /usr/include/g++-3/stl_relops.h
SummarySpectrumW.o: /usr/include/g++-3/stl_pair.h
SummarySpectrumW.o: /usr/include/g++-3/type_traits.h /usr/include/string.h
SummarySpectrumW.o: /usr/include/stdlib.h
SummarySpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new.h
SummarySpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/new
SummarySpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/exception
SummarySpectrumW.o: /usr/include/g++-3/iostream.h
SummarySpectrumW.o: /usr/include/g++-3/streambuf.h /usr/include/libio.h
SummarySpectrumW.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
SummarySpectrumW.o: /usr/include/g++-3/stl_iterator.h
SummarySpectrumW.o: /usr/include/g++-3/stl_construct.h
SummarySpectrumW.o: /usr/include/g++-3/stl_uninitialized.h
SummarySpectrumW.o: /usr/include/g++-3/stl_tempbuf.h
SummarySpectrumW.o: /usr/include/g++-3/stl_algo.h
SummarySpectrumW.o: /usr/include/g++-3/stl_heap.h /usr/include/assert.h
