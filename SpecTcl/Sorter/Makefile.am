INCLUDES = -I. -I../Utility -I../Exception -I../Xamine -I../Gates -I../Events -I../Display @TCL_FLAGS@
LDADD = -lSorting -lXplus -lGates -lAnalysis -lEventSource -ltclPlus -lException -lXamine -lXt -lX11
LDFLAGS = -L. @TCL_LDFLAGS@ @XTLIBSW@ @XLIBSW@

lib_LTLIBRARIES = libSorting.la

libSorting_la_SOURCES = DictionaryException.cpp Histogrammer.cpp Spectrum.cpp \
	Spectrum1DL.cpp Spectrum2DW.cpp Spectrum1DW.cpp Spectrum2DB.cpp \
	BitSpectrumW.cpp BitSpectrumL.cpp SummarySpectrumW.cpp \
	SummarySpectrumB.cpp SpectrumFactory.cpp SpectrumFactoryException.cpp \
	SnapshotSpectrum.cpp Gamma1DW.cpp Gamma1DL.cpp Gamma2DW.cpp \
	Gamma2DB.cpp GateMediator.cpp MSpectrum1DW.cpp MSpectrum1DL.cpp \
	MSpectrum2DW.cpp MSpectrum2DB.cpp MGamma1DW.cpp \
	MGamma1DL.cpp MGamma2DB.cpp MGamma2DW.cpp

include_HEADERS = BitSpectrumL.h BitSpectrumW.h Dictionary.h \
	DictionaryException.h Gamma1DL.h Gamma1DW.h Gamma2DB.h Gamma2DW.h \
	GateMediator.h Histogrammer.h MGamma1DL.h MGamma1DW.h MGamma2DB.h \
	MGamma2DW.h MSpectrum1DL.h MSpectrum1DW.h MSpectrum2DB.h \
	MSpectrum2DW.h Parameter.h SnapshotSpectrum.h Spectrum.h \
	Spectrum1DL.h Spectrum1DW.h Spectrum2DB.h Spectrum2DW.h \
	SpectrumFactory.h SpectrumFactoryException.h SummarySpectrumB.h \
	SummarySpectrumW.h

EXTRA_DIST = Sorter.WC Sorter.omt
