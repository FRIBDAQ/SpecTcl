
# Make a man page from xml... note that the
# touch is done just in case the docbook man page creation
# command does not exist... in that case an empty man page
# is created so that the installer doesn't have to be so
# smart (although in fact it is.).



SUFFIXES =  .xml .3

.xml.3:
	-$(MANDOCBOOK) man $< 2>/dev/null
	touch $@

DEFS = @WCHAR_T_DEF@
INCLUDES = -I.. -I. @TCL_FLAGS@ -I../Utility -I../NSCLException @XINCLUDES@
LDADD = -ltclPlus -lException -lXt -lX11 $(X11EXTRA_LDFLAGS) -L. @TCL_LDFLAGS@


lib_LTLIBRARIES = libtclPlus.la

libtclPlus_la_LDFLAGS = -version-info $(SOVERSION):0 \
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


XMLMANSRC=CTCLApplication.xml CTCLInterpreterObject.xml \
	CTCLInterpreter.xml CTCLVariable.xml CTCLChannel.xml \
	CTCLCommandPackage.xml CTCLCompatibilityProcessor.xml \
	CTCLException.xml CTCLFileHandler.xml CTCLHashTable.xml \
	CTCLHashTableItem.xml CTCLHashTableIterator.xml CTCLIdleProcess.xml \
	CTCLList.xml CTCLObject.xml CTCLObjectProcessor.xml \
	CTCLPackagedCommand.xml CTCLProcessor.xml CTCLResult.xml \
	CTCLString.xml CTCLTimer.xml TclPlus.xml

XMLMANS=CTCLApplication.3 CTCLInterpreterObject.3 CTCLInterpreter.3 \
	CTCLVariable.3 CTCLChannel.3 CTCLCommandPackage.3 \
	CTCLCompatibilityProcessor.3 CTCLException.3 CTCLFileHandler.3 \
	CTCLHashTable.3 CTCLHashTableItem.3 CTCLHashTableIterator.3 \
	CTCLIdleProcess.3 \
	CTCLList.3 CTCLObject.3 CTCLObjectProcessor.3 \
	CTCLPackagedCommand.3 CTCLProcessor.3 CTCLResult.3 \
	CTCLString.3 CTCLTimer.3 TclPlus.3

manpages: $(XMLMANS) $(XMLMANSRC)

install-data-local: manpages
	$(mkinstalldirs) $(prefix)/man
	$(mkinstalldirs) $(prefix)/man/man3
	for f in  $(XMLMANS); do                          \
            if [ -e $$f ]; then                                \
		$(INSTALL_DATA) $$f @prefix@/man/man3;         \
	   fi;						      \
	done


clean-local:
	-rm -f $(XMLMANS)

man_MANS = TracingIntro.3 CTracedVariable.3  CVariableTraceCallback.3 \
	 CLanguageTraceCallback.3

EXTRA_DIST = TCL.WC TCL.omt tkAppInit.c $(man_MANS) $(XMLMANSRC)

if CPPUNIT

noinst_PROGRAMS = tracetests interptests

noinst_HEADERS = Asserts.h



tracetests_SOURCES = TestRunner.cpp \
			ccallbacktest.cpp \
			tracevartest.cpp

tracetests_DEPENDENCIES = $(testableObjects)

TestLdflags = $(testableObjects) $(CPPUNIT_LDFLAGS) $(X11EXTRA_LDFLAGS) \
	-L@top_srcdir@/NSCLException -lException @TCL_LDFLAGS@


tracetests_LDADD = $(TestLdflags)

interptests_SOURCES=interpretertest.cpp objecttest.cpp resulttest.cpp \
	objcommandtest.cpp compatcommand.cpp


interptests_LDADD = TestRunner.o $(TestLdflags)


interptests_DEPENDENCIES = $(testableObjects) TestRunner.o


TESTS = ./tracetests ./interptests

endif
