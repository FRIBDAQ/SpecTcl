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

TCLApplication.o: TCLApplication.h TCLInterpreterObject.h
TCLApplication.o: /usr/local/include/tcl.h /usr/include/stdio.h
TCLApplication.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLApplication.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLApplication.o: /usr/include/mach/machine/vm_types.h
TCLApplication.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLApplication.o: /usr/include/sys/limits.h
TCLApplication.o: /usr/include/sys/machine/machlimits.h
TCLApplication.o: /usr/include/sys/syslimits.h
TCLApplication.o: /usr/include/sys/machine/machtime.h
TCLApplication.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLApplication.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLApplication.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLApplication.o: /usr/include/strings.h /usr/local/include/tk.h
TCLApplication.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
TCLApplication.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TCLApplication.o: /usr/include/stddef.h /usr/local/include/tkDecls.h
TCLCommandPackage.o: TCLCommandPackage.h TCLInterpreterObject.h
TCLCommandPackage.o: /usr/local/include/tcl.h /usr/include/stdio.h
TCLCommandPackage.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLCommandPackage.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLCommandPackage.o: /usr/include/mach/machine/vm_types.h
TCLCommandPackage.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLCommandPackage.o: /usr/include/sys/limits.h
TCLCommandPackage.o: /usr/include/sys/machine/machlimits.h
TCLCommandPackage.o: /usr/include/sys/syslimits.h
TCLCommandPackage.o: /usr/include/sys/machine/machtime.h
TCLCommandPackage.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLCommandPackage.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLCommandPackage.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLCommandPackage.o: /usr/include/strings.h TCLProcessor.h TCLResult.h
TCLException.o: TCLException.h TCLInterpreterObject.h
TCLException.o: /usr/local/include/tcl.h /usr/include/stdio.h
TCLException.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLException.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLException.o: /usr/include/mach/machine/vm_types.h
TCLException.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLException.o: /usr/include/sys/limits.h
TCLException.o: /usr/include/sys/machine/machlimits.h
TCLException.o: /usr/include/sys/syslimits.h
TCLException.o: /usr/include/sys/machine/machtime.h
TCLException.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLException.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLException.o: TCLResult.h TCLInterpreter.h TCLString.h
TCLException.o: /usr/include/string.h /usr/include/strings.h
TCLFileHandler.o: TCLFileHandler.h TCLInterpreterObject.h
TCLFileHandler.o: /usr/local/include/tcl.h /usr/include/stdio.h
TCLFileHandler.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLFileHandler.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLFileHandler.o: /usr/include/mach/machine/vm_types.h
TCLFileHandler.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLFileHandler.o: /usr/include/sys/limits.h
TCLFileHandler.o: /usr/include/sys/machine/machlimits.h
TCLFileHandler.o: /usr/include/sys/syslimits.h
TCLFileHandler.o: /usr/include/sys/machine/machtime.h
TCLFileHandler.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLFileHandler.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLFileHandler.o: /usr/include/unistd.h /usr/include/sys/access.h
TCLFileHandler.o: /usr/local/include/tk.h /usr/include/X11/Xlib.h
TCLFileHandler.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLFileHandler.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
TCLFileHandler.o: /usr/local/include/tkDecls.h
TCLIdleProcess.o: TCLIdleProcess.h TCLTimer.h TCLInterpreterObject.h
TCLIdleProcess.o: /usr/local/include/tcl.h /usr/include/stdio.h
TCLIdleProcess.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLIdleProcess.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLIdleProcess.o: /usr/include/mach/machine/vm_types.h
TCLIdleProcess.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLIdleProcess.o: /usr/include/sys/limits.h
TCLIdleProcess.o: /usr/include/sys/machine/machlimits.h
TCLIdleProcess.o: /usr/include/sys/syslimits.h
TCLIdleProcess.o: /usr/include/sys/machine/machtime.h
TCLIdleProcess.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLIdleProcess.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLIdleProcess.o: /usr/local/include/tk.h /usr/include/X11/Xlib.h
TCLIdleProcess.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLIdleProcess.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
TCLIdleProcess.o: /usr/local/include/tkDecls.h
TCLInterpreter.o: TCLInterpreter.h TCLString.h /usr/local/include/tcl.h
TCLInterpreter.o: /usr/include/stdio.h /usr/include/standards.h
TCLInterpreter.o: /usr/include/sys/seek.h /usr/include/va_list.h
TCLInterpreter.o: /usr/include/sys/types.h
TCLInterpreter.o: /usr/include/mach/machine/vm_types.h
TCLInterpreter.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLInterpreter.o: /usr/include/sys/limits.h
TCLInterpreter.o: /usr/include/sys/machine/machlimits.h
TCLInterpreter.o: /usr/include/sys/syslimits.h
TCLInterpreter.o: /usr/include/sys/machine/machtime.h
TCLInterpreter.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLInterpreter.o: /usr/local/include/tclDecls.h /usr/include/string.h
TCLInterpreter.o: /usr/include/strings.h TCLException.h
TCLInterpreter.o: TCLInterpreterObject.h /usr/include/assert.h TCLResult.h
TCLInterpreterObject.o: TCLInterpreterObject.h /usr/local/include/tcl.h
TCLInterpreterObject.o: /usr/include/stdio.h /usr/include/standards.h
TCLInterpreterObject.o: /usr/include/sys/seek.h /usr/include/va_list.h
TCLInterpreterObject.o: /usr/include/sys/types.h
TCLInterpreterObject.o: /usr/include/mach/machine/vm_types.h
TCLInterpreterObject.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLInterpreterObject.o: /usr/include/sys/limits.h
TCLInterpreterObject.o: /usr/include/sys/machine/machlimits.h
TCLInterpreterObject.o: /usr/include/sys/syslimits.h
TCLInterpreterObject.o: /usr/include/sys/machine/machtime.h
TCLInterpreterObject.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLInterpreterObject.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLList.o: TCLList.h TCLInterpreterObject.h /usr/local/include/tcl.h
TCLList.o: /usr/include/stdio.h /usr/include/standards.h
TCLList.o: /usr/include/sys/seek.h /usr/include/va_list.h
TCLList.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
TCLList.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLList.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
TCLList.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
TCLList.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLList.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLList.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLList.o: /usr/include/strings.h /usr/include/malloc.h
TCLObject.o: TCLObject.h TCLInterpreterObject.h /usr/local/include/tcl.h
TCLObject.o: /usr/include/stdio.h /usr/include/standards.h
TCLObject.o: /usr/include/sys/seek.h /usr/include/va_list.h
TCLObject.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
TCLObject.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLObject.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
TCLObject.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
TCLObject.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLObject.o: /usr/local/include/tclDecls.h /usr/include/assert.h TCLList.h
TCLObject.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLObject.o: /usr/include/strings.h TCLException.h TCLResult.h
TCLProcessor.o: TCLProcessor.h TCLInterpreter.h TCLString.h
TCLProcessor.o: /usr/local/include/tcl.h /usr/include/stdio.h
TCLProcessor.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLProcessor.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLProcessor.o: /usr/include/mach/machine/vm_types.h
TCLProcessor.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLProcessor.o: /usr/include/sys/limits.h
TCLProcessor.o: /usr/include/sys/machine/machlimits.h
TCLProcessor.o: /usr/include/sys/syslimits.h
TCLProcessor.o: /usr/include/sys/machine/machtime.h
TCLProcessor.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLProcessor.o: /usr/local/include/tclDecls.h /usr/include/string.h
TCLProcessor.o: /usr/include/strings.h TCLResult.h TCLException.h
TCLProcessor.o: TCLInterpreterObject.h /usr/include/assert.h
TCLResult.o: TCLResult.h TCLString.h /usr/local/include/tcl.h
TCLResult.o: /usr/include/stdio.h /usr/include/standards.h
TCLResult.o: /usr/include/sys/seek.h /usr/include/va_list.h
TCLResult.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
TCLResult.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLResult.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
TCLResult.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
TCLResult.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLResult.o: /usr/local/include/tclDecls.h /usr/include/string.h
TCLResult.o: /usr/include/strings.h TCLInterpreter.h /usr/include/assert.h
TCLString.o: TCLString.h /usr/local/include/tcl.h /usr/include/stdio.h
TCLString.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLString.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLString.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
TCLString.o: /usr/include/getopt.h /usr/include/sys/limits.h
TCLString.o: /usr/include/sys/machine/machlimits.h
TCLString.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
TCLString.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLString.o: /usr/local/include/tclDecls.h /usr/include/string.h
TCLString.o: /usr/include/strings.h
TCLTest.o: /usr/local/include/tk.h /usr/include/X11/Xlib.h
TCLTest.o: /usr/include/sys/types.h /usr/include/standards.h
TCLTest.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
TCLTest.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLTest.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
TCLTest.o: /usr/local/include/tkDecls.h TCLApplication.h
TCLTest.o: TCLInterpreterObject.h /usr/local/include/tcl.h
TCLTest.o: /usr/include/stdio.h /usr/include/sys/seek.h
TCLTest.o: /usr/include/va_list.h /usr/include/getopt.h
TCLTest.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
TCLTest.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
TCLTest.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLTest.o: /usr/local/include/tclDecls.h /usr/include/assert.h TCLVariable.h
TCLTest.o: TCLProcessor.h TCLInterpreter.h TCLString.h /usr/include/string.h
TCLTest.o: /usr/include/strings.h TCLResult.h /usr/include/fcntl.h
TCLTest.o: /usr/include/sys/fcntl.h /usr/include/sys/mode.h
TCLTest.o: /usr/include/sys/stat.h
TCLTimer.o: TCLTimer.h TCLInterpreterObject.h /usr/local/include/tcl.h
TCLTimer.o: /usr/include/stdio.h /usr/include/standards.h
TCLTimer.o: /usr/include/sys/seek.h /usr/include/va_list.h
TCLTimer.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
TCLTimer.o: /usr/include/sys/select.h /usr/include/getopt.h
TCLTimer.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
TCLTimer.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
TCLTimer.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLTimer.o: /usr/local/include/tclDecls.h /usr/include/assert.h
TCLTimer.o: /usr/local/include/tk.h /usr/include/X11/Xlib.h
TCLTimer.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLTimer.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
TCLTimer.o: /usr/local/include/tkDecls.h
TCLVariable.o: TCLVariable.h TCLInterpreter.h TCLString.h
TCLVariable.o: /usr/local/include/tcl.h /usr/include/stdio.h
TCLVariable.o: /usr/include/standards.h /usr/include/sys/seek.h
TCLVariable.o: /usr/include/va_list.h /usr/include/sys/types.h
TCLVariable.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
TCLVariable.o: /usr/include/getopt.h /usr/include/sys/limits.h
TCLVariable.o: /usr/include/sys/machine/machlimits.h
TCLVariable.o: /usr/include/sys/syslimits.h
TCLVariable.o: /usr/include/sys/machine/machtime.h
TCLVariable.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TCLVariable.o: /usr/local/include/tclDecls.h /usr/include/string.h
TCLVariable.o: /usr/include/strings.h /usr/include/assert.h
