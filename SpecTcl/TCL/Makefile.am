DEFS = @WCHAR_T_DEF@
INCLUDES = -I.. -I. @TCL_FLAGS@ -I../Utility -I../NSCLException @XINCLUDES@
LDADD = -ltclPlus -lException -lXt -lX11 $(X11EXTRA_LDFLAGS)
LDFLAGS = -L. @TCL_LDFLAGS@ 

lib_LTLIBRARIES = libtclPlus.la

libtclPlus_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

testableSources = TCLInterpreter.cpp TCLProcessor.cpp  \
	TCLVariable.cpp TCLString.cpp  \
	TCLResult.cpp TCLTimer.cpp TCLFileHandler.cpp \
	TCLList.cpp TCLException.cpp TCLInterpreterObject.cpp \
	TCLCommandPackage.cpp TCLIdleProcess.cpp \
	TCLObject.cpp TCLChannel.cpp TCLTracedVariable.cpp \
	CLanguageTraceCallbacks.cpp


libtclPlus_la_SOURCES = TCLApplication.cpp $(testableSources)



include_HEADERS = TCLApplication.h TCLCommandPackage.h TCLException.h \
	TCLFileHandler.h TCLHashTable.h TCLHashTableItem.h \
	TCLHashTableIterator.h TCLIdleProcess.h	TCLInterpreter.h \
	TCLInterpreterObject.h TCLList.h TCLObject.h TCLPackagedCommand.h \
	TCLProcessor.h TCLResult.h TCLString.h TCLTimer.h TCLVariable.h \
	TCLChannel.h TCLVersionHacks.h TCLTracedVariable.h \
	VariableTraceCallback.h CLanguageTraceCallbacks.h


if CPPUNIT

noinst_PROGRAMS = tracetests

noinst_HEADERS = Asserts.h

tracetests_SOURCES = TestRunner.cpp \
			ccallbacktest.cpp \
			tracevartest.cpp  \
			$(testableSources)

tracetests_LDADD = $(CPPUNIT_LDFLAGS) $(X11EXTRA_LDFLAGS) \
	-L@top_srcdir@/NSCLException -lException

TESTS = ./tracetests

endif

man_MANS = TracingIntro.3 CTracedVariable.3  CVariableTraceCallback.3 \
	 CLanguageTraceCallback.3

EXTRA_DIST = TCL.WC TCL.omt tkAppInit.c $(man_MANS)
