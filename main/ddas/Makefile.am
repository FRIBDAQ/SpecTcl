
lib_LTLIBRARIES = libDDASUnpacker.la 

libDDASUnpacker_la_SOURCES = DDASUnpacker.cpp \
	DDASBuiltUnpacker.cpp  \
	 DDASHit.cpp DDASHitUnpacker.cpp CFileDrivenParameterMapper.cpp \
	CCalibratedFileDrivenParameterMapper.cpp FitHitUnpacker.cpp \
	functions.cpp lmfit.cpp DDASBuiltFitUnpacker.cpp

include_HEADERS = DDASUnpacker.h \
	DDASBuiltUnpacker.h \
	ParameterMapper.h \
	DDASHit.h DDASHitUnpacker.h \
	DDASBitMasks.h CFileDrivenParameterMapper.h \
	CCalibratedFileDrivenParameterMapper.h \
	DDASFitHit.h FitHitUnpacker.h functions.h lmfit.h DDASBuiltFitUnpacker.h \
	FitParameterMapper.h

noinst_HEADERS=FitExtender.h




libDDASUnpacker_la_CXXFLAGS = -I@top_srcdir@/Core \
	-I@top_srcdir@/Utility	\
	-I@top_srcdir@/Core    \
	@LIBTCLPLUS_CFLAGS@	\
	@TCL_CPPFLAGS@ @ROOT_CFLAGS@



libDDASUnpacker_la_LIBADD = @top_builddir@/Core/libTclGrammerApp.la \
	@ROOT_LDFLAGS@						\
	-lgsl @LIBEXCEPTION_LDFLAGS@  @TCL_LIBS@ 


noinst_PROGRAMS = unittests

unittests_SOURCES = TestRunner.cpp \
		DDASHitTest.cpp \
		DDASUnpackerTest.cpp \
		DDASUnpackerTest250.cpp \
		DDASUnpackerTest250MSPS16Bit.cpp \
		DDASUnpackerTest500.cpp	\
		fileunpackertests.cpp ddascalibmaptest.cpp


unittests_CXXFLAGS = @CPPUNIT_CFLAGS@ @ROOT_CFLAGS@ -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core @LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ 
unittests_LDADD = @builddir@/libDDASUnpacker.la @top_builddir@/Core/libTclGrammerApp.la  \
	@ROOT_LDFLAGS@  @CPPUNIT_LIBS@ @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@
TESTS_ENVIRONMENT=LD_LIBRARY_PATH=$(ROOT_LIBRARY_DIR)
TESTS = ./unittests


SUBDIRS=src
