
#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321
#


AM_CPPFLAGS = 	-I@top_srcdir@/Utility	\
		-I@top_srcdir@/Display	\
		-I@top_srcdir@/factories \
		-I@top_srcdir@/Core \
		@LIBTCLPLUS_CFLAGS@ \
		@TCL_CPPFLAGS@ \
		@TK_CPPFLAGS@

if CPPUNIT
noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp SpectrumDictionaryFitObserverTests.cpp

TestRunner_CXXFLAGS=@CPPUNIT_CFLAGS@

TestRunner_LDADD = @CPPUNIT_LIBS@ \
		    @top_builddir@/Tape/libtape.la \
		    @top_builddir@/Core/libSpecTcl.la \
		    @LIBTCLPLUS_LDFLAGS@ \
		    @CPPUNIT_LIBS@ \
		    @TK_LIBS@ \
		    @TCL_LIBS@ \
		    -lgsl -lgslcblas

check-TESTS:
	./TestRunner

endif
