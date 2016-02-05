

if CPPUNIT
noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp TestHistogrammer.cpp

TestRunner_CPPFLAGS =	-I@top_srcdir@/Utility \
			-I@top_srcdir@/Core \
			@TCL_CPPFLAGS@ \
			@LIBTCLPLUS_CFLAGS@

TestRunner_CXXFLAGS=@CPPUNIT_CFLAGS@

TestRunner_LDADD = @CPPUNIT_LIBS@ \
			@top_builddir@/Core/libTclGrammerApp.la \
		    @top_builddir@/Tape/libtape.la \
		    @LIBTCLPLUS_LDFLAGS@ \
		    @TCL_LIBS@ \
		    -lgsl -lgslcblas


check-TESTS:
	./TestRunner

endif
