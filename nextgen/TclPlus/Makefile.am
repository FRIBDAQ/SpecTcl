

ACLOCAL_AMFLAGS=-I m4



INCLUDES = -I.. -I. @TCL_FLAGS@ -I../Utility -I@top_srcdir@/NSCLException 
LDADD = -ltclPlus @top_srcdir/NSCLException/libException.la \
	-lXt -lX11 $(X11EXTRA_LDFLAGS) -L. @TCL_LDFLAGS@


lib_LTLIBRARIES = libtclPlus.la

libtclPlus_la_LDFLAGS =  \
	-Wl,"-rpath-link=$(libdir)" @TCL_LDFLAGS@

testableSources = TCLInterpreter.cpp TCLProcessor.cpp  \
	TCLVariable.cpp TCLString.cpp  \
	TCLResult.cpp TCLTimer.cpp TCLFileHandler.cpp \
	TCLList.cpp TCLException.cpp TCLInterpreterObject.cpp \
	TCLCommandPackage.cpp TCLIdleProcess.cpp \
	TCLObject.cpp TCLChannel.cpp TCLTracedVariable.cpp \
	CLanguageTraceCallbacks.cpp TCLObjectProcessor.cpp \
	TCLCompatibilityProcessor.cpp 

testableObjects = TCLInterpreter.o   TCLProcessor.o    \
	TCLVariable.o   TCLString.o    \
	TCLResult.o   TCLTimer.o   TCLFileHandler.o   \
	TCLList.o   TCLException.o \
	TCLInterpreterObject.o   \
	TCLCommandPackage.o   TCLIdleProcess.o   \
	TCLObject.o   TCLChannel.o   TCLTracedVariable.o   \
	CLanguageTraceCallbacks.o  TCLObjectProcessor.o \
	TCLCompatibilityProcessor.o

libtclPlus_la_SOURCES = TCLApplication.cpp $(testableSources)



include_HEADERS = TCLApplication.h TCLCommandPackage.h TCLException.h \
	TCLFileHandler.h TCLHashTable.h TCLHashTableItem.h \
	TCLHashTableIterator.h TCLIdleProcess.h	TCLInterpreter.h \
	TCLInterpreterObject.h TCLList.h TCLObject.h TCLPackagedCommand.h \
	TCLProcessor.h TCLResult.h TCLString.h TCLTimer.h TCLVariable.h \
	TCLChannel.h TCLVersionHacks.h TCLTracedVariable.h \
	VariableTraceCallback.h CLanguageTraceCallbacks.h \
	TCLObjectProcessor.h TCLCompatibilityProcessor.h 



noinst_PROGRAMS = tracetests interptests

noinst_HEADERS = Asserts.h



tracetests_SOURCES = TestRunner.cpp \
			ccallbacktest.cpp \
			tracevartest.cpp

tracetests_DEPENDENCIES = $(testableObjects)

TestLdflags = $(testableObjects) $(CPPUNIT_LIBS) \
	-L@top_srcdir@/NSCLException -lException @TCL_LDFLAGS@


tracetests_LDADD = $(TestLdflags)

interptests_SOURCES=interpretertest.cpp objecttest.cpp resulttest.cpp \
	objcommandtest.cpp compatcommand.cpp


interptests_LDADD = TestRunner.o $(TestLdflags)


interptests_DEPENDENCIES = $(testableObjects) TestRunner.o


TESTS = ./tracetests ./interptests

