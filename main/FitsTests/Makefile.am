
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


AM_CPPFLAGS = 	-I@top_srcdir@/Fits\
		-I@top_srcdir@/Utility	\
		-I@top_srcdir@/Xamine	\
		-I@top_srcdir@/NSCLException \
		-I@top_srcdir@/Sorter    \
		-I@top_srcdir@/SpecTcl	\
		-I@top_srcdir@/Events	\
		-I@top_srcdir@/EventSource \
		-I@top_srcdir@/Display	\
		-I@top_srcdir@/Gates	\
		-I@top_srcdir@/Filter	\
		-I@top_srcdir@/TCL	\
		-I@top_srcdir@/factories \
		@LIBTCLPLUS_CFLAGS@ \
		@TCL_CPPFLAGS@ \
		@TK_CPPFLAGS@

if CPPUNIT
noinst_PROGRAMS=TestRunner

TestRunner_SOURCES=TestRunner.cpp SpectrumDictionaryFitObserverTests.cpp

TestRunner_CXXFLAGS=@CPPUNIT_CFLAGS@

TestRunner_LDADD = @CPPUNIT_LIBS@ \
		    @top_builddir@/Fits/libFits.la \
		    @top_builddir@/Sorter/libSorting.la \
		    @top_builddir@/Tape/libtape.la \
		    @top_builddir@/SpecTcl/libTclGrammerCommands.la \
		    @top_builddir@/Events/libAnalysis.la \
			    @top_builddir@/Analysis/libBufferAnalysis.la \
			    @top_builddir@/SpecTcl/libTclGrammerCommands.la \
			    @top_builddir@/Filter/libFilter.la	\
			    @top_builddir@/TreeParam/libTreeParam.la \
			    @top_builddir@/DecoderRing/libDecoderRing.la \
			    @top_builddir@/SpectrumIO/libSpecio.la \
			    @top_builddir@/Xamine/libXplus.la \
			    @top_builddir@/DisplayInterface/libDisplayInterface.la \
			    @LIBTCLPLUS_LDFLAGS@ \
			    @CPPUNIT_LIBS@ \
			    @TK_LIBS@ \
			    @TCL_LIBS@

check-TESTS:
	./TestRunner

endif
