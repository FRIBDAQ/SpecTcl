
lib_LTLIBRARIES = libDDASUnpacker.la 

libDDASUnpacker_la_SOURCES = DDASUnpacker.cpp \
	DDASBuiltUnpacker.cpp FragmentIndex.cpp  \
	fragment.h DDASHit.cpp DDASHitUnpacker.cpp

include_HEADERS = DDASUnpacker.h \
	DDASBuiltUnpacker.h \
	ParameterMapper.h \
	DDASHit.h DDASHitUnpacker.h \
	FragmentIndex.h DDASBitMasks.h \
	fragment.h 



libDDASUnpacker_la_CXXFLAGS = -I@top_srcdir@/Core \
	-I@top_srcdir@/Utility	\
	-I@top_srcdir@/SpecTcl \
	-I@top_srcdir@/Events	\
	-I@top_srcdir@/Filter   \
	-I@top_srcdir@/EventSource \
	-I@top_srcdir@/DecoderRing \
	-I@top_srcdir@/TCL	\
	-I@top_srcdir@/NSCLException \
	@TCL_FLAGS@ -std=c++11



libDDASUnpacker_la_LIBADD = @top_builddir@/SpecTcl/libTclGrammerCommands.la \
	@top_builddir@/Sorter/libSorting.la				\
	@top_builddir@/Xamine/libXplus.la				\
	@top_builddir@/Gates/libGates.la 				\
	@top_builddir@/Events/libAnalysis.la				\
	@top_builddir@/Filter/libFilter.la				\
	@top_builddir@/Analysis/libBufferAnalysis.la			\
	@top_builddir@/EventSource/libEventSource.la			\
	@top_builddir@/SpectrumIO/libSpecio.la				\
	@top_builddir@/TreeParam/libTreeParam.la			\
	@top_builddir@/Sorter/libSorting.la		\
	@top_builddir@/DecoderRing/libDecoderRing.la	\
	@top_builddir@/Fits/libFits.la			\
	@top_builddir@/Tape/libtape.la			\
	@top_builddir@/TCL/libtclPlus.la		\
	@top_builddir@/NSCLException/libException.la    \
	-lXt -lX11 $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS)  \
	@TCL_LDFLAGS@ @XLIBSW@


noinst_PROGRAMS = unittests

unittests_SOURCES = TestRunner.cpp \
		DDASHitTest.cpp \
		DDASUnpackerTest.cpp \
		DDASUnpackerTest250.cpp \
		DDASUnpackerTest250MSPS16Bit.cpp \
		DDASUnpackerTest500.cpp \
		Asserts.h

unittests_CXXFLAGS = @CPPUNIT_INCLUDES@ -std=c++11 -I@top_srcdir@/SpecTcl

unittests_LDADD = @builddir@/libDDASUnpacker.la $(libDDASUnpacker_la_LIBADD)  -lcppunit

TESTS = ./unittests


SUBDIRS=src
