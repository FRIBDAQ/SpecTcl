#
#  Makefile for the C++ interface to Tcl/TK
#  The following targets are supported:
#      system(default)         - Build the library.
#      install                 - Install the system at $(INSTDIR)
#      clean                   - Clean up .o and .a files.
#      tests                   - Build any test programs against local version
#                                of the library.

LinuxLIBS=-ldl

HPUXLIBDIR=-L/opt/tcltk/lib
HPUXINCS=-I/opt/tcltk/include

DarwinLIBS=-L/usr/local/lib
DarwinINCS=-I/usr/include -I/usr/local/include -I/usr/X11R6/include \
	-I/usr/include/sys

CYGWININCS=-I/usr/local/tcl/include
CYGWINLIBDIR=-L/usr/local/tcl/lib


INCLUDEDIRS= $($(OS)INCS) -I. -I$(INSTDIR)/Include -I/usr/local/include 
OBJECTS=TCLInterpreter.o TCLProcessor.o  \
	TCLVariable.o TCLString.o  \
	TCLResult.o TCLTimer.o TCLFileHandler.o \
	TCLList.o TCLException.o TCLInterpreterObject.o TCLApplication.o \
	TCLCommandPackage.o TCLIdleProcess.o TCLObject.o

LIBDIRS=$($(OS)LIBDIR) -L/usr/local -L. -L$(INSTDIR)/Lib

LIBS=   -ltclPlus -lException -ltk -ltcl -lXt -lX11 $($(OS)LIBS)


ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr


system: libtclPlus.a
	$(ECHO) Library rebuild complete.

clean: 
	$(RM) $(OBJECTS) 
	$(RM) libtclPlus.a

depend:
	makedepend $(INCLUDEDIRS) *.cpp

install: system
	$(INSTALL) libtclPlus.a $(INSTDIR)/Lib
	$(INSTALL) *.h          $(INSTDIR)/Include

tests: TCLTest 
	$(ECHO) Tests rebuilt.

TCLTest: TCLTest.cpp system
	$(LINK) -o TCLTest TCLTest.cpp $(LIBS)

libtclPlus.a: $(OBJECTS)
	$(ARCHIVE) libtclPlus.a $(OBJECTS)

TCLApplication.o: TCLApplication.cpp
	$(COMPILE) TCLApplication.cpp

TCLInterpreter.o: TCLInterpreter.cpp
	$(COMPILE) TCLInterpreter.cpp

TCLProcessor.o: TCLProcessor.cpp
	$(COMPILE) TCLProcessor.cpp


TCLVariable.o: TCLVariable.cpp
	$(COMPILE) TCLVariable.cpp

TCLString.o: TCLString.cpp
	$(COMPILE) TCLString.cpp

TCLResult.o: TCLResult.cpp
	$(COMPILE) TCLResult.cpp

TCLTimer.o: TCLTimer.cpp
	$(COMPILE) TCLTimer.cpp

TCLFileHandler.o: TCLFileHandler.cpp
	$(COMPILE) TCLFileHandler.cpp

TCLList.o: TCLList.cpp
	$(COMPILE) TCLList.cpp

TCLException.o: TCLException.cpp
	$(COMPILE) TCLException.cpp

TCLInterpreterObject.o: TCLInterpreterObject.cpp
	$(COMPILE) TCLInterpreterObject.cpp


TCLCommandPackage.o: TCLCommandPackage.cpp
	$(COMPILE) TCLCommandPackage.cpp

TCLIdleProcess.o: TCLIdleProcess.cpp
	$(COMPILE) TCLIdleProcess.cpp

TCLObject.o: TCLObject.cpp
	$(COMPILE) TCLObject.cpp

# DO NOT DELETE

TCLApplication.o: TCLApplication.h TCLInterpreterObject.h
TCLApplication.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLApplication.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLApplication.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLApplication.o: /usr/local/tcl/include/tclDecls.h /usr/include/assert.h
TCLApplication.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLApplication.o: /usr/local/tcl/include/tk.h
TCLApplication.o: /usr/local/tcl/include/X11/Xlib.h
TCLApplication.o: /usr/local/tcl/include/X11/X.h
TCLApplication.o: /usr/local/tcl/include/X11/Xfuncproto.h
TCLApplication.o: /usr/local/tcl/include/tkIntXlibDecls.h
TCLApplication.o: /usr/local/tcl/include/X11/Xutil.h
TCLApplication.o: /usr/local/tcl/include/tkDecls.h
TCLCommandPackage.o: TCLCommandPackage.h TCLInterpreterObject.h
TCLCommandPackage.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLCommandPackage.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLCommandPackage.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLCommandPackage.o: /usr/local/tcl/include/tclDecls.h /usr/include/assert.h
TCLCommandPackage.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLCommandPackage.o: TCLProcessor.h TCLResult.h
TCLException.o: TCLException.h TCLInterpreterObject.h
TCLException.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLException.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLException.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLException.o: /usr/local/tcl/include/tclDecls.h /usr/include/assert.h
TCLException.o: TCLResult.h TCLInterpreter.h TCLString.h
TCLException.o: /usr/include/string.h
TCLFileHandler.o: TCLFileHandler.h TCLInterpreterObject.h
TCLFileHandler.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLFileHandler.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLFileHandler.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLFileHandler.o: /usr/local/tcl/include/tclDecls.h /usr/include/assert.h
TCLFileHandler.o: /usr/include/unistd.h /usr/include/sys/unistd.h
TCLFileHandler.o: /usr/include/sys/types.h /usr/include/machine/types.h
TCLFileHandler.o: /usr/include/sys/features.h /usr/include/getopt.h
TCLFileHandler.o: /usr/local/tcl/include/tk.h
TCLFileHandler.o: /usr/local/tcl/include/X11/Xlib.h
TCLFileHandler.o: /usr/local/tcl/include/X11/X.h
TCLFileHandler.o: /usr/local/tcl/include/X11/Xfuncproto.h
TCLFileHandler.o: /usr/local/tcl/include/tkIntXlibDecls.h
TCLFileHandler.o: /usr/local/tcl/include/X11/Xutil.h
TCLFileHandler.o: /usr/local/tcl/include/tkDecls.h
TCLIdleProcess.o: TCLIdleProcess.h TCLTimer.h TCLInterpreterObject.h
TCLIdleProcess.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLIdleProcess.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLIdleProcess.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLIdleProcess.o: /usr/local/tcl/include/tclDecls.h /usr/include/assert.h
TCLIdleProcess.o: /usr/local/tcl/include/tk.h
TCLIdleProcess.o: /usr/local/tcl/include/X11/Xlib.h
TCLIdleProcess.o: /usr/local/tcl/include/X11/X.h
TCLIdleProcess.o: /usr/local/tcl/include/X11/Xfuncproto.h
TCLIdleProcess.o: /usr/local/tcl/include/tkIntXlibDecls.h
TCLIdleProcess.o: /usr/local/tcl/include/X11/Xutil.h
TCLIdleProcess.o: /usr/local/tcl/include/tkDecls.h
TCLInterpreter.o: TCLInterpreter.h TCLString.h /usr/local/tcl/include/tcl.h
TCLInterpreter.o: /usr/include/stdio.h /usr/include/_ansi.h
TCLInterpreter.o: /usr/include/sys/config.h /usr/include/sys/reent.h
TCLInterpreter.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
TCLInterpreter.o: /usr/include/string.h TCLException.h TCLInterpreterObject.h
TCLInterpreter.o: /usr/include/assert.h TCLResult.h
TCLInterpreterObject.o: TCLInterpreterObject.h /usr/local/tcl/include/tcl.h
TCLInterpreterObject.o: /usr/include/stdio.h /usr/include/_ansi.h
TCLInterpreterObject.o: /usr/include/sys/config.h /usr/include/sys/reent.h
TCLInterpreterObject.o: /usr/include/sys/_types.h
TCLInterpreterObject.o: /usr/local/tcl/include/tclDecls.h
TCLInterpreterObject.o: /usr/include/assert.h
TCLList.o: TCLList.h TCLInterpreterObject.h /usr/local/tcl/include/tcl.h
TCLList.o: /usr/include/stdio.h /usr/include/_ansi.h
TCLList.o: /usr/include/sys/config.h /usr/include/sys/reent.h
TCLList.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
TCLList.o: /usr/include/assert.h TCLInterpreter.h TCLString.h
TCLList.o: /usr/include/string.h /usr/include/malloc.h
TCLObject.o: TCLObject.h TCLInterpreterObject.h /usr/local/tcl/include/tcl.h
TCLObject.o: /usr/include/stdio.h /usr/include/_ansi.h
TCLObject.o: /usr/include/sys/config.h /usr/include/sys/reent.h
TCLObject.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
TCLObject.o: /usr/include/assert.h TCLList.h TCLInterpreter.h TCLString.h
TCLObject.o: /usr/include/string.h TCLException.h TCLResult.h
TCLProcessor.o: TCLProcessor.h TCLInterpreter.h TCLString.h
TCLProcessor.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLProcessor.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLProcessor.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLProcessor.o: /usr/local/tcl/include/tclDecls.h /usr/include/string.h
TCLProcessor.o: TCLResult.h TCLException.h TCLInterpreterObject.h
TCLProcessor.o: /usr/include/assert.h
TCLResult.o: TCLResult.h TCLString.h /usr/local/tcl/include/tcl.h
TCLResult.o: /usr/include/stdio.h /usr/include/_ansi.h
TCLResult.o: /usr/include/sys/config.h /usr/include/sys/reent.h
TCLResult.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
TCLResult.o: /usr/include/string.h TCLInterpreter.h /usr/include/assert.h
TCLString.o: TCLString.h /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLString.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLString.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLString.o: /usr/local/tcl/include/tclDecls.h /usr/include/string.h
TCLTest.o: /usr/local/tcl/include/tk.h /usr/local/tcl/include/X11/Xlib.h
TCLTest.o: /usr/local/tcl/include/X11/X.h
TCLTest.o: /usr/local/tcl/include/X11/Xfuncproto.h
TCLTest.o: /usr/local/tcl/include/tkIntXlibDecls.h
TCLTest.o: /usr/local/tcl/include/X11/Xutil.h
TCLTest.o: /usr/local/tcl/include/tkDecls.h TCLApplication.h
TCLTest.o: TCLInterpreterObject.h /usr/local/tcl/include/tcl.h
TCLTest.o: /usr/include/stdio.h /usr/include/_ansi.h
TCLTest.o: /usr/include/sys/config.h /usr/include/sys/reent.h
TCLTest.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
TCLTest.o: /usr/include/assert.h TCLVariable.h TCLProcessor.h
TCLTest.o: TCLInterpreter.h TCLString.h /usr/include/string.h TCLResult.h
TCLTest.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
TCLTest.o: /usr/include/sys/types.h /usr/include/machine/types.h
TCLTest.o: /usr/include/sys/stat.h /usr/include/time.h
TCLTest.o: /usr/include/machine/time.h /usr/include/sys/features.h
TCLTimer.o: TCLTimer.h TCLInterpreterObject.h /usr/local/tcl/include/tcl.h
TCLTimer.o: /usr/include/stdio.h /usr/include/_ansi.h
TCLTimer.o: /usr/include/sys/config.h /usr/include/sys/reent.h
TCLTimer.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
TCLTimer.o: /usr/include/assert.h /usr/local/tcl/include/tk.h
TCLTimer.o: /usr/local/tcl/include/X11/Xlib.h /usr/local/tcl/include/X11/X.h
TCLTimer.o: /usr/local/tcl/include/X11/Xfuncproto.h
TCLTimer.o: /usr/local/tcl/include/tkIntXlibDecls.h
TCLTimer.o: /usr/local/tcl/include/X11/Xutil.h
TCLTimer.o: /usr/local/tcl/include/tkDecls.h
TCLVariable.o: TCLVariable.h TCLInterpreter.h TCLString.h
TCLVariable.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TCLVariable.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLVariable.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLVariable.o: /usr/local/tcl/include/tclDecls.h /usr/include/string.h
TCLVariable.o: /usr/include/assert.h
