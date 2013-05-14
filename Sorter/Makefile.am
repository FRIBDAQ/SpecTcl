INCLUDES = -I. -I../Utility @LIBTCLPLUS_CFLAGS@ -I../Xamine -I../Gates \
	   -I../Events -I../Display @TCL_CPPFLAGS@ -I.. -I@top_srcdir@/Fits 

libSorting_la_LIBADD =  @top_srcdir@/Xamine/libXplus.la 	\
			@top_srcdir@/Gates/libGates.la 		\
			@top_srcdir@/Events/libAnalysis.la 	\
			@top_srcdir@/EventSource/libEventSource.la \
			@LIBTCLPLUS_LDFLAGS@	\
			@top_srcdir@/Display/libXamine.la	



lib_LTLIBRARIES = libSorting.la
libSorting_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath=$(libdir)"

libSorting_la_SOURCES = DictionaryException.cpp Histogrammer.cpp Spectrum.cpp \
	Spectrum1DL.cpp Spectrum2DW.cpp Spectrum1DW.cpp Spectrum2DB.cpp \
	BitSpectrumW.cpp BitSpectrumL.cpp SummarySpectrumW.cpp \
	SummarySpectrumB.cpp SpectrumFactory.cpp SpectrumFactoryException.cpp \
	SnapshotSpectrum.cpp Gamma1DW.cpp Gamma1DL.cpp Gamma2DW.cpp \
	CGammaSpectrum.cpp \
	Gamma2DB.cpp GateMediator.cpp CAxis.cpp CParameterMapping.cpp  Parameter.cpp \
	SpectrumS.cpp \
	Spectrum2DL.cpp Gamma2DL.cpp  SummarySpectrumL.cpp \
	CSpectrum2Dm.cpp CSpectrum2DmL.cpp CSpectrum2DmW.cpp CSpectrum2DmB.cpp \
	CHistogrammerFitObserver.cpp Gamma2DD.cpp CFlattenedGateList.cpp CSpectrumByParameter.cpp \
	CGammaSummarySpectrum.cpp

include_HEADERS = BitSpectrumL.h BitSpectrumW.h Dictionary.h \
	DictionaryException.h Gamma1DL.h Gamma1DW.h Gamma2DB.h Gamma2DW.h \
	GateMediator.h Histogrammer.h \
	Parameter.h SnapshotSpectrum.h Spectrum.h \
	CGammaSpectrum.h \
	Spectrum1DL.h Spectrum1DW.h Spectrum2DB.h Spectrum2DW.h \
	SpectrumFactory.h SpectrumFactoryException.h SummarySpectrumB.h \
	SummarySpectrumW.h CAxis.h CParameterMapping.h SpectrumS.h \
	Spectrum2DL.h  Gamma2DL.h SummarySpectrumL.h \
	CSpectrum2Dm.h CSpectrum2DmL.h CSpectrum2DmW.h CSpectrum2DmB.h \
	CHistogrammerFitObserver.h Gamma2DD.h DynamicList.h DynamicList.cpp \
	CFlattenedGateList.h CSpectrumByParameter.h CGammaSummarySpectrum.h

EXTRA_DIST = Sorter.WC Sorter.omt


if CPPUNIT
noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp TestDictObservers.cpp

TestRunner_CXXFLAGS=@CPPUNIT_CFLAGS@

TestRunner_LDADD = @CPPUNIT_LIBS@

noinst_HEADERS = Asserts.h

TESTS=./TestRunner

endif