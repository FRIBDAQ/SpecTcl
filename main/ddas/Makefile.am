
lib_LTLIBRARIES = libDDASUnpacker.la 

libDDASUnpacker_la_SOURCES = DDASUnpacker.cpp \
	DDASBuiltUnpacker.cpp FragmentIndex.cpp  \
	fragment.h DDASHit.cpp DDASHitUnpacker.cpp

include_HEADERS = DDASUnpacker.h \
	DDASBuiltUnpacker.h \
	ParameterMapper.h \
	DDASHit.h DDASHitUnpacker.h \
	FragmentIndex.h



libDDASUnpacker_la_CXXFLAGS = -I@top_srcdir@/Core \
	-I@top_srcdir@/Utility	\
	@LIBTCLPLUS_CFLAGS@	\
	@TCL_CPPFLAGS@



libDDASUnpacker_la_LIBADD = @top_builddir@/Core/libTclGrammerApp.la \
	@LIBEXCEPTION_LDFLAGS@  @TCL_LIBS@ 


noinst_PROGRAMS = unittests

unittests_SOURCES = TestRunner.cpp \
		DDASHitTest.cpp \
		DDASUnpackerTest.cpp \
		DDASUnpackerTest250.cpp \
		DDASUnpackerTest250MSPS16Bit.cpp \
		DDASUnpackerTest500.cpp \
		Asserts.h

unittests_CXXFLAGS = @CPPUNIT_CFLAGS@
unittests_LDADD = @builddir@/libDDASUnpacker.la  @CPPUNIT_LIBS@

TESTS = ./unittests


SUBDIRS=src
