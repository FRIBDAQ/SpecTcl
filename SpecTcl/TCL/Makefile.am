DEFS = @WCHAR_T_DEF@
INCLUDES = -I.. -I. @TCL_FLAGS@ -I../Utility -I../NSCLException @XINCLUDES@
LDADD = -ltclPlus -lException -lXt -lX11 $(X11EXTRA_LDFLAGS)
LDFLAGS = -L. @TCL_LDFLAGS@ 

lib_LTLIBRARIES = libtclPlus.la

libtclPlus_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libtclPlus_la_SOURCES = TCLInterpreter.cpp TCLProcessor.cpp  \
	TCLVariable.cpp TCLString.cpp  \
	TCLResult.cpp TCLTimer.cpp TCLFileHandler.cpp \
	TCLList.cpp TCLException.cpp TCLInterpreterObject.cpp \
	TCLApplication.cpp TCLCommandPackage.cpp TCLIdleProcess.cpp \
	TCLObject.cpp TCLChannel.cpp

include_HEADERS = TCLApplication.h TCLCommandPackage.h TCLException.h \
	TCLFileHandler.h TCLHashTable.h TCLHashTableItem.h \
	TCLHashTableIterator.h TCLIdleProcess.h	TCLInterpreter.h \
	TCLInterpreterObject.h TCLList.h TCLObject.h TCLPackagedCommand.h \
	TCLProcessor.h TCLResult.h TCLString.h TCLTimer.h TCLVariable.h \
	TCLChannel.h TCLVersionHacks.h

EXTRA_DIST = TCL.WC TCL.omt tkAppInit.c
