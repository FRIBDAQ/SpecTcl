INCLUDES = -I$(top_srcdir)/include \
	-I$(top_srcdir)/include/exception \
	-I$(top_srcdir)/include/tclplus \
	@TCL_CPPFLAGS@ @CPPUNIT_CFLAGS@ @PTHREAD_CFLAGS@

LDADD = -ltclPlus -lException
#-lXt -lX11 $(X11EXTRA_LDFLAGS)
AM_LDFLAGS = -L. @TCL_LIBS@ -L$(top_srcdir)/exception 

lib_LTLIBRARIES = libtclPlus.la

libtclPlus_la_LDFLAGS = -version-info $(SOVERSION) \
	-Wl,"-rpath-link=$(libdir)" @PTHREAD_LIBS@

libtclPlus_la_CXXFLAGS = @PTHREAD_CFLAGS@ $(AM_CXXFLAGS)

testableSources = TCLInterpreter.cpp TCLProcessor.cpp  \
	TCLVariable.cpp TCLString.cpp  \
	TCLResult.cpp TCLTimer.cpp TCLFileHandler.cpp \
	TCLList.cpp TCLException.cpp TCLInterpreterObject.cpp \
	TCLCommandPackage.cpp TCLIdleProcess.cpp \
	TCLObject.cpp TCLChannel.cpp TCLTracedVariable.cpp \
	CLanguageTraceCallbacks.cpp TCLObjectProcessor.cpp \
	TCLCompatibilityProcessor.cpp TCLLiveEventLoop.cpp	\
	CTCLChannelCommander.cpp CTCLStdioCommander.cpp	\
	CTCLTcpServerInstance.cpp CTCLServer.cpp TCLObjectPackage.cpp \
	TCLPackagedObjectProcessor.cpp
# CItemConfiguration.cpp  CConfigurableObject.cpp



testableObjects = TCLInterpreter.lo   TCLProcessor.lo    \
	TCLVariable.lo   TCLString.lo    \
	TCLResult.lo   TCLTimer.lo   TCLFileHandler.lo   \
	TCLList.lo   TCLException.lo \
	TCLInterpreterObject.lo   \
	TCLCommandPackage.lo   TCLIdleProcess.lo   \
	TCLObject.lo   TCLChannel.lo   TCLTracedVariable.lo   \
	CLanguageTraceCallbacks.lo  TCLObjectProcessor.lo \
	TCLCompatibilityProcessor.lo TCLPackagedObjectProcessor.lo \
	TCLObjectPackage.lo

libtclPlus_la_SOURCES = TCLApplication.cpp $(testableSources)

include_HEADERS = \
	$(top_srcdir)/include/tclplus/TCLApplication.h \
	$(top_srcdir)/include/tclplus/TCLCommandPackage.h \
	$(top_srcdir)/include/tclplus/TCLException.h \
	$(top_srcdir)/include/tclplus/TCLFileHandler.h \
	$(top_srcdir)/include/tclplus/TCLHashTable.h \
	$(top_srcdir)/include/tclplus/TCLHashTableItem.h \
	$(top_srcdir)/include/tclplus/TCLHashTableIterator.h \
	$(top_srcdir)/include/tclplus/TCLIdleProcess.h	\
	$(top_srcdir)/include/tclplus/TCLInterpreter.h \
	$(top_srcdir)/include/tclplus/TCLInterpreterObject.h \
	$(top_srcdir)/include/tclplus/TCLList.h \
	$(top_srcdir)/include/tclplus/TCLObject.h \
	$(top_srcdir)/include/tclplus/TCLPackagedCommand.h \
	$(top_srcdir)/include/tclplus/TCLProcessor.h \
	$(top_srcdir)/include/tclplus/TCLResult.h \
	$(top_srcdir)/include/tclplus/TCLString.h \
	$(top_srcdir)/include/tclplus/TCLTimer.h \
	$(top_srcdir)/include/tclplus/TCLVariable.h \
	$(top_srcdir)/include/tclplus/TCLChannel.h \
	$(top_srcdir)/include/tclplus/TCLVersionHacks.h \
	$(top_srcdir)/include/tclplus/TCLTracedVariable.h \
	$(top_srcdir)/include/tclplus/VariableTraceCallback.h \
	$(top_srcdir)/include/tclplus/CLanguageTraceCallbacks.h \
	$(top_srcdir)/include/tclplus/TCLObjectProcessor.h \
	$(top_srcdir)/include/tclplus/TCLCompatibilityProcessor.h 	\
	$(top_srcdir)/include/tclplus/TCLLiveEventLoop.h \
	$(top_srcdir)/include/tclplus/CTCLChannelCommander.h	\
	$(top_srcdir)/include/tclplus/CTCLStdioCommander.h \
	$(top_srcdir)/include/tclplus/CTCLTcpServerInstance.h \
	$(top_srcdir)/include/tclplus/CTCLServer.h	\
	$(top_srcdir)/include/tclplus/TCLObjectPackage.h \
	$(top_srcdir)/include/tclplus/TCLPackagedObjectProcessor.h \
	$(top_srcdir)/include/tclplus/CItemConfiguration.h \
	$(top_srcdir)/include/tclplus/CConfigurableObject.h

noinst_HEADERS = $(top_srcdir)/include/tclplus/Asserts.h \
	$(top_srcdir)/include/daqdatatypes.h

# evttclsh - a tclsh that has an event loop constantly running.
bin_PROGRAMS		=	evttclsh

evttclsh_SOURCES	= 	evttclsh.cpp
evttclsh_DEPENDENCIES	= 	libtclPlus.la

evttclsh_LDADD		= 	libtclPlus.la \
				@top_srcdir@/exception/libException.la \
				@TCL_LIBS@
evttclsh_LDFLAGS	= 	-Wl,"-rpath-link=$(libdir)" @TCL_LIBS@


clean-local:
	-rm -f $(XMLMANS)

EXTRA_DIST = tkAppInit.c tclplus.xml

### disabled for now, will have fox hook these back up
 noinst_PROGRAMS = tracetests interptests tcpservertest

tracetests_SOURCES = TestRunner.cpp \
 			ccallbacktest.cpp \
 			tracevartest.cpp

tracetests_DEPENDENCIES = $(testableObjects)

 TestLdflags = $(testableObjects) $(CPPUNIT_LDFLAGS) \
 	-L@top_srcdir@/exception -lException
 # $(X11EXTRA_LDFLAGS)

 tracetests_LDADD = $(TestLdflags) @TCL_LIBS@ @CPPUNIT_LIBS@

 interptests_SOURCES=interpretertest.cpp objecttest.cpp resulttest.cpp \
 	objcommandtest.cpp compatcommand.cpp packagetests.cpp

 interptests_LDADD = TestRunner.lo $(TestLdflags) @CPPUNIT_LIBS@


interptests_DEPENDENCIES = $(testableObjects) TestRunner.lo
tcpservertest_SOURCES	= 	tcpservertest.cpp
tcpservertest_DEPENDENCIES	= 	libtclPlus.la
tcpservertest_LDADD		= 	libtclPlus.la	\
 				@top_srcdir@/exception/libException.la \
 			@TCL_LIBS@ @CPPUNIT_LIBS@
tcpservertest_LDFLAGS	= 	-Wl,"-rpath-link=$(libdir)" @TCL_LIBS@ @CPPUNIT_LIBS@

 TESTS = ./tracetests ./interptests
