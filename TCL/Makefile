#
#  Makefile for the C++ interface to Tcl/TK
#  The following targets are supported:
#      system(default)         - Build the library.
#      install                 - Install the system at $(INSTDIR)
#      clean                   - Clean up .o and .a files.
#      tests                   - Build any test programs against local version
#                                of the library.

LinuxLIBS=-ldl
HPUXLIBS=-L/opt/tcltk/lib
HPUXINCS=-I/opt/tcltk/include

INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include $($(OS)INCS)
OBJECTS=TCLInterpreter.o TCLProcessor.o  \
	TCLVariable.o TCLString.o  \
	TCLResult.o TCLTimer.o TCLFileHandler.o \
	TCLList.o TCLException.o TCLInterpreterObject.o TCLApplication.o \
	TCLCommandPackage.o TCLIdleProcess.o TCLObject.o
LIBDIRS=-L/usr/local -L. -L$(INSTDIR)/Lib
LIBS=   -ltclPlus -lException -ltk -ltcl -lXt -lX11 $($(OS)LIBS)


ECHO=echo
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS)
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
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
TCLApplication.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLApplication.o: /usr/include/bits/types.h /usr/include/libio.h
TCLApplication.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLApplication.o: /usr/include/assert.h TCLInterpreter.h TCLString.h
TCLApplication.o: /usr/include/string.h /usr/include/tk.h
TCLApplication.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
TCLApplication.o: /usr/include/time.h /usr/include/endian.h
TCLApplication.o: /usr/include/bits/endian.h /usr/include/sys/select.h
TCLApplication.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
TCLApplication.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
TCLApplication.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLCommandPackage.o: TCLCommandPackage.h TCLInterpreterObject.h
TCLCommandPackage.o: /usr/include/tcl.h /usr/include/stdio.h
TCLCommandPackage.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLCommandPackage.o: /usr/include/gnu/stubs.h /usr/include/stddef.h
TCLCommandPackage.o: /usr/include/stdarg.h /usr/include/bits/types.h
TCLCommandPackage.o: /usr/include/libio.h /usr/include/_G_config.h
TCLCommandPackage.o: /usr/include/bits/stdio_lim.h /usr/include/assert.h
TCLCommandPackage.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLCommandPackage.o: TCLProcessor.h TCLResult.h
TCLException.o: TCLException.h TCLInterpreterObject.h /usr/include/tcl.h
TCLException.o: /usr/include/stdio.h /usr/include/features.h
TCLException.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLException.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLException.o: /usr/include/bits/types.h /usr/include/libio.h
TCLException.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLException.o: /usr/include/assert.h TCLResult.h TCLInterpreter.h
TCLException.o: TCLString.h /usr/include/string.h
TCLFileHandler.o: TCLFileHandler.h TCLInterpreterObject.h /usr/include/tcl.h
TCLFileHandler.o: /usr/include/stdio.h /usr/include/features.h
TCLFileHandler.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLFileHandler.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLFileHandler.o: /usr/include/bits/types.h /usr/include/libio.h
TCLFileHandler.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLFileHandler.o: /usr/include/assert.h /usr/include/unistd.h
TCLFileHandler.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
TCLFileHandler.o: /usr/include/getopt.h /usr/include/tk.h
TCLFileHandler.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
TCLFileHandler.o: /usr/include/time.h /usr/include/endian.h
TCLFileHandler.o: /usr/include/bits/endian.h /usr/include/sys/select.h
TCLFileHandler.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
TCLFileHandler.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
TCLFileHandler.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLIdleProcess.o: TCLIdleProcess.h TCLTimer.h TCLInterpreterObject.h
TCLIdleProcess.o: /usr/include/tcl.h /usr/include/stdio.h
TCLIdleProcess.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLIdleProcess.o: /usr/include/gnu/stubs.h /usr/include/stddef.h
TCLIdleProcess.o: /usr/include/stdarg.h /usr/include/bits/types.h
TCLIdleProcess.o: /usr/include/libio.h /usr/include/_G_config.h
TCLIdleProcess.o: /usr/include/bits/stdio_lim.h /usr/include/assert.h
TCLIdleProcess.o: /usr/include/tk.h /usr/include/X11/Xlib.h
TCLIdleProcess.o: /usr/include/sys/types.h /usr/include/time.h
TCLIdleProcess.o: /usr/include/endian.h /usr/include/bits/endian.h
TCLIdleProcess.o: /usr/include/sys/select.h /usr/include/bits/select.h
TCLIdleProcess.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
TCLIdleProcess.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLIdleProcess.o: /usr/include/X11/Xosdefs.h
TCLInterpreter.o: TCLInterpreter.h TCLString.h /usr/include/tcl.h
TCLInterpreter.o: /usr/include/stdio.h /usr/include/features.h
TCLInterpreter.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLInterpreter.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLInterpreter.o: /usr/include/bits/types.h /usr/include/libio.h
TCLInterpreter.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLInterpreter.o: /usr/include/string.h TCLException.h TCLInterpreterObject.h
TCLInterpreter.o: /usr/include/assert.h TCLResult.h
TCLInterpreterObject.o: TCLInterpreterObject.h /usr/include/tcl.h
TCLInterpreterObject.o: /usr/include/stdio.h /usr/include/features.h
TCLInterpreterObject.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLInterpreterObject.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLInterpreterObject.o: /usr/include/bits/types.h /usr/include/libio.h
TCLInterpreterObject.o: /usr/include/_G_config.h
TCLInterpreterObject.o: /usr/include/bits/stdio_lim.h /usr/include/assert.h
TCLList.o: TCLList.h TCLInterpreterObject.h /usr/include/tcl.h
TCLList.o: /usr/include/stdio.h /usr/include/features.h
TCLList.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLList.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLList.o: /usr/include/bits/types.h /usr/include/libio.h
TCLList.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLList.o: /usr/include/assert.h TCLInterpreter.h TCLString.h
TCLList.o: /usr/include/string.h /usr/include/malloc.h
TCLObject.o: TCLObject.h TCLInterpreterObject.h /usr/include/tcl.h
TCLObject.o: /usr/include/stdio.h /usr/include/features.h
TCLObject.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLObject.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLObject.o: /usr/include/bits/types.h /usr/include/libio.h
TCLObject.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLObject.o: /usr/include/assert.h TCLList.h TCLInterpreter.h TCLString.h
TCLObject.o: /usr/include/string.h TCLException.h TCLResult.h
TCLProcessor.o: TCLProcessor.h TCLInterpreter.h TCLString.h
TCLProcessor.o: /usr/include/tcl.h /usr/include/stdio.h
TCLProcessor.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLProcessor.o: /usr/include/gnu/stubs.h /usr/include/stddef.h
TCLProcessor.o: /usr/include/stdarg.h /usr/include/bits/types.h
TCLProcessor.o: /usr/include/libio.h /usr/include/_G_config.h
TCLProcessor.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
TCLProcessor.o: TCLResult.h TCLException.h TCLInterpreterObject.h
TCLProcessor.o: /usr/include/assert.h
TCLResult.o: TCLResult.h TCLString.h /usr/include/tcl.h /usr/include/stdio.h
TCLResult.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLResult.o: /usr/include/gnu/stubs.h /usr/include/stddef.h
TCLResult.o: /usr/include/stdarg.h /usr/include/bits/types.h
TCLResult.o: /usr/include/libio.h /usr/include/_G_config.h
TCLResult.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
TCLResult.o: TCLInterpreter.h /usr/include/assert.h
TCLString.o: TCLString.h /usr/include/tcl.h /usr/include/stdio.h
TCLString.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLString.o: /usr/include/gnu/stubs.h /usr/include/stddef.h
TCLString.o: /usr/include/stdarg.h /usr/include/bits/types.h
TCLString.o: /usr/include/libio.h /usr/include/_G_config.h
TCLString.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
TCLTest.o: /usr/include/tk.h /usr/include/X11/Xlib.h /usr/include/sys/types.h
TCLTest.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLTest.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
TCLTest.o: /usr/include/stddef.h /usr/include/time.h /usr/include/endian.h
TCLTest.o: /usr/include/bits/endian.h /usr/include/sys/select.h
TCLTest.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
TCLTest.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
TCLTest.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLTest.o: TCLApplication.h TCLInterpreterObject.h /usr/include/tcl.h
TCLTest.o: /usr/include/stdio.h /usr/include/stdarg.h /usr/include/libio.h
TCLTest.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLTest.o: /usr/include/assert.h TCLVariable.h TCLProcessor.h
TCLTest.o: TCLInterpreter.h TCLString.h /usr/include/string.h TCLResult.h
TCLTest.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
TCLTest.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
TCLTimer.o: TCLTimer.h TCLInterpreterObject.h /usr/include/tcl.h
TCLTimer.o: /usr/include/stdio.h /usr/include/features.h
TCLTimer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLTimer.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLTimer.o: /usr/include/bits/types.h /usr/include/libio.h
TCLTimer.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLTimer.o: /usr/include/assert.h /usr/include/tk.h /usr/include/X11/Xlib.h
TCLTimer.o: /usr/include/sys/types.h /usr/include/time.h
TCLTimer.o: /usr/include/endian.h /usr/include/bits/endian.h
TCLTimer.o: /usr/include/sys/select.h /usr/include/bits/select.h
TCLTimer.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
TCLTimer.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLTimer.o: /usr/include/X11/Xosdefs.h
TCLVariable.o: TCLVariable.h TCLInterpreter.h TCLString.h /usr/include/tcl.h
TCLVariable.o: /usr/include/stdio.h /usr/include/features.h
TCLVariable.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLVariable.o: /usr/include/stddef.h /usr/include/stdarg.h
TCLVariable.o: /usr/include/bits/types.h /usr/include/libio.h
TCLVariable.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLVariable.o: /usr/include/string.h /usr/include/assert.h
