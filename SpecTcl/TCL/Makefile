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

TCLApplication.o: TCLApplication.h TCLInterpreterObject.h /usr/include/tcl.h
TCLApplication.o: /usr/include/stdio.h /usr/include/features.h
TCLApplication.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLApplication.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLApplication.o: /usr/include/bits/types.h /usr/include/libio.h
TCLApplication.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLApplication.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLApplication.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLApplication.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLApplication.o: /usr/include/assert.h TCLInterpreter.h TCLString.h
TCLApplication.o: /usr/include/string.h /usr/include/tk.h
TCLApplication.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
TCLApplication.o: /usr/include/time.h /usr/include/X11/X.h
TCLApplication.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLApplication.o: /usr/include/tkDecls.h
TCLCommandPackage.o: TCLCommandPackage.h TCLInterpreterObject.h
TCLCommandPackage.o: /usr/include/tcl.h /usr/include/stdio.h
TCLCommandPackage.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLCommandPackage.o: /usr/include/gnu/stubs.h
TCLCommandPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLCommandPackage.o: /usr/include/bits/types.h /usr/include/libio.h
TCLCommandPackage.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLCommandPackage.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLCommandPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLCommandPackage.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLCommandPackage.o: /usr/include/assert.h TCLInterpreter.h TCLString.h
TCLCommandPackage.o: /usr/include/string.h TCLProcessor.h TCLResult.h
TCLException.o: TCLException.h TCLInterpreterObject.h /usr/include/tcl.h
TCLException.o: /usr/include/stdio.h /usr/include/features.h
TCLException.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLException.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLException.o: /usr/include/bits/types.h /usr/include/libio.h
TCLException.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLException.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLException.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLException.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLException.o: /usr/include/assert.h TCLResult.h TCLInterpreter.h
TCLException.o: TCLString.h /usr/include/string.h
TCLFileHandler.o: TCLFileHandler.h TCLInterpreterObject.h /usr/include/tcl.h
TCLFileHandler.o: /usr/include/stdio.h /usr/include/features.h
TCLFileHandler.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLFileHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLFileHandler.o: /usr/include/bits/types.h /usr/include/libio.h
TCLFileHandler.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLFileHandler.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLFileHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLFileHandler.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLFileHandler.o: /usr/include/assert.h /usr/include/unistd.h
TCLFileHandler.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
TCLFileHandler.o: /usr/include/tk.h /usr/include/X11/Xlib.h
TCLFileHandler.o: /usr/include/sys/types.h /usr/include/time.h
TCLFileHandler.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLFileHandler.o: /usr/include/X11/Xosdefs.h /usr/include/tkDecls.h
TCLIdleProcess.o: TCLIdleProcess.h TCLTimer.h TCLInterpreterObject.h
TCLIdleProcess.o: /usr/include/tcl.h /usr/include/stdio.h
TCLIdleProcess.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLIdleProcess.o: /usr/include/gnu/stubs.h
TCLIdleProcess.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLIdleProcess.o: /usr/include/bits/types.h /usr/include/libio.h
TCLIdleProcess.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLIdleProcess.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLIdleProcess.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLIdleProcess.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLIdleProcess.o: /usr/include/assert.h /usr/include/tk.h
TCLIdleProcess.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
TCLIdleProcess.o: /usr/include/time.h /usr/include/X11/X.h
TCLIdleProcess.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLIdleProcess.o: /usr/include/tkDecls.h
TCLInterpreter.o: TCLInterpreter.h TCLString.h /usr/include/tcl.h
TCLInterpreter.o: /usr/include/stdio.h /usr/include/features.h
TCLInterpreter.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLInterpreter.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLInterpreter.o: /usr/include/bits/types.h /usr/include/libio.h
TCLInterpreter.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLInterpreter.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLInterpreter.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLInterpreter.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLInterpreter.o: /usr/include/string.h TCLException.h TCLInterpreterObject.h
TCLInterpreter.o: /usr/include/assert.h TCLResult.h
TCLInterpreterObject.o: TCLInterpreterObject.h /usr/include/tcl.h
TCLInterpreterObject.o: /usr/include/stdio.h /usr/include/features.h
TCLInterpreterObject.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLInterpreterObject.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLInterpreterObject.o: /usr/include/bits/types.h /usr/include/libio.h
TCLInterpreterObject.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLInterpreterObject.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLInterpreterObject.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLInterpreterObject.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLInterpreterObject.o: /usr/include/assert.h
TCLList.o: TCLList.h TCLInterpreterObject.h /usr/include/tcl.h
TCLList.o: /usr/include/stdio.h /usr/include/features.h
TCLList.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLList.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLList.o: /usr/include/bits/types.h /usr/include/libio.h
TCLList.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLList.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLList.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLList.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLList.o: /usr/include/assert.h TCLInterpreter.h TCLString.h
TCLList.o: /usr/include/string.h /usr/include/malloc.h
TCLObject.o: TCLObject.h TCLInterpreterObject.h /usr/include/tcl.h
TCLObject.o: /usr/include/stdio.h /usr/include/features.h
TCLObject.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLObject.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLObject.o: /usr/include/bits/types.h /usr/include/libio.h
TCLObject.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLObject.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLObject.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLObject.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLObject.o: /usr/include/assert.h TCLList.h TCLInterpreter.h TCLString.h
TCLObject.o: /usr/include/string.h TCLException.h TCLResult.h
TCLProcessor.o: TCLProcessor.h TCLInterpreter.h TCLString.h
TCLProcessor.o: /usr/include/tcl.h /usr/include/stdio.h
TCLProcessor.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLProcessor.o: /usr/include/gnu/stubs.h
TCLProcessor.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLProcessor.o: /usr/include/bits/types.h /usr/include/libio.h
TCLProcessor.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLProcessor.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLProcessor.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLProcessor.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLProcessor.o: /usr/include/string.h TCLResult.h TCLException.h
TCLProcessor.o: TCLInterpreterObject.h /usr/include/assert.h
TCLResult.o: TCLResult.h TCLString.h /usr/include/tcl.h /usr/include/stdio.h
TCLResult.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLResult.o: /usr/include/gnu/stubs.h
TCLResult.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLResult.o: /usr/include/bits/types.h /usr/include/libio.h
TCLResult.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLResult.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLResult.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLResult.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLResult.o: /usr/include/string.h TCLInterpreter.h /usr/include/assert.h
TCLString.o: TCLString.h /usr/include/tcl.h /usr/include/stdio.h
TCLString.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLString.o: /usr/include/gnu/stubs.h
TCLString.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLString.o: /usr/include/bits/types.h /usr/include/libio.h
TCLString.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLString.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLString.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLString.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLString.o: /usr/include/string.h
TCLTest.o: /usr/include/tk.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
TCLTest.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLTest.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
TCLTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLTest.o: /usr/include/time.h /usr/include/X11/X.h
TCLTest.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLTest.o: /usr/include/tkDecls.h TCLApplication.h TCLInterpreterObject.h
TCLTest.o: /usr/include/tcl.h /usr/include/stdio.h /usr/include/libio.h
TCLTest.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLTest.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLTest.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLTest.o: /usr/include/assert.h TCLVariable.h TCLProcessor.h
TCLTest.o: TCLInterpreter.h TCLString.h /usr/include/string.h TCLResult.h
TCLTest.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
TCLTest.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
TCLTimer.o: TCLTimer.h TCLInterpreterObject.h /usr/include/tcl.h
TCLTimer.o: /usr/include/stdio.h /usr/include/features.h
TCLTimer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLTimer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLTimer.o: /usr/include/bits/types.h /usr/include/libio.h
TCLTimer.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLTimer.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLTimer.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLTimer.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLTimer.o: /usr/include/assert.h /usr/include/tk.h /usr/include/X11/Xlib.h
TCLTimer.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/X11/X.h
TCLTimer.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLTimer.o: /usr/include/tkDecls.h
TCLVariable.o: TCLVariable.h TCLInterpreter.h TCLString.h /usr/include/tcl.h
TCLVariable.o: /usr/include/stdio.h /usr/include/features.h
TCLVariable.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLVariable.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLVariable.o: /usr/include/bits/types.h /usr/include/libio.h
TCLVariable.o: /usr/include/_G_config.h /usr/include/wchar.h
TCLVariable.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TCLVariable.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLVariable.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TCLVariable.o: /usr/include/string.h /usr/include/assert.h
