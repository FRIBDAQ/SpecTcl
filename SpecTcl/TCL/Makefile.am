DEFS = @WCHAR_T_DEF@
INCLUDES = -I. @TCL_FLAGS@ -I../Utility -I../Exception
LDADD = -ltclPlus -lException -lXt -lX11
LDFLAGS = -L. @TCL_LDFLAGS@

lib_LTLIBRARIES = libtclPlus.la

libtclPlus_la_SOURCES = TCLInterpreter.cpp TCLProcessor.cpp  \
	TCLVariable.cpp TCLString.cpp  \
	TCLResult.cpp TCLTimer.cpp TCLFileHandler.cpp \
	TCLList.cpp TCLException.cpp TCLInterpreterObject.cpp \
	TCLApplication.cpp TCLCommandPackage.cpp TCLIdleProcess.cpp \
	TCLObject.cpp

include_HEADERS = TCLApplication.h TCLCommandPackage.h TCLException.h \
	TCLFileHandler.h TCLHashTable.h TCLHashTableItem.h \
	TCLHashTableIterator.h TCLIdleProcess.h	TCLInterpreter.h \
	TCLInterpreterObject.h TCLList.h TCLObject.h TCLPackagedCommand.h \
	TCLProcessor.h TCLResult.h TCLString.h TCLTimer.h TCLVariable.h

EXTRA_DIST = TCL.WC TCL.omt tkAppInit.c
