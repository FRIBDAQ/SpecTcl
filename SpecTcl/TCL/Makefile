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
TCLApplication.o: /usr/include/assert.h /usr/include/features.h
TCLApplication.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLApplication.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLApplication.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLCommandPackage.o: TCLCommandPackage.h TCLInterpreterObject.h
TCLCommandPackage.o: /usr/include/assert.h /usr/include/features.h
TCLCommandPackage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLCommandPackage.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLCommandPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLCommandPackage.o: TCLProcessor.h TCLResult.h
TCLException.o: TCLException.h TCLInterpreterObject.h /usr/include/assert.h
TCLException.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLException.o: /usr/include/gnu/stubs.h TCLResult.h TCLInterpreter.h
TCLException.o: TCLString.h /usr/include/string.h
TCLException.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLFileHandler.o: TCLFileHandler.h TCLInterpreterObject.h
TCLFileHandler.o: /usr/include/assert.h /usr/include/features.h
TCLFileHandler.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLFileHandler.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
TCLFileHandler.o: /usr/include/bits/types.h
TCLFileHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLFileHandler.o: /usr/include/bits/confname.h /usr/include/stdio.h
TCLFileHandler.o: /usr/include/libio.h /usr/include/_G_config.h
TCLFileHandler.o: /usr/include/wchar.h /usr/include/bits/wchar.h
TCLFileHandler.o: /usr/include/gconv.h
TCLFileHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TCLFileHandler.o: /usr/include/bits/stdio_lim.h
TCLIdleProcess.o: TCLIdleProcess.h TCLTimer.h TCLInterpreterObject.h
TCLIdleProcess.o: /usr/include/assert.h /usr/include/features.h
TCLIdleProcess.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLInterpreter.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLInterpreter.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLInterpreter.o: /usr/include/gnu/stubs.h
TCLInterpreter.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLInterpreter.o: TCLException.h TCLInterpreterObject.h /usr/include/assert.h
TCLInterpreter.o: TCLResult.h
TCLInterpreterObject.o: TCLInterpreterObject.h /usr/include/assert.h
TCLInterpreterObject.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLInterpreterObject.o: /usr/include/gnu/stubs.h
TCLList.o: TCLList.h TCLInterpreterObject.h /usr/include/assert.h
TCLList.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLList.o: /usr/include/gnu/stubs.h TCLInterpreter.h TCLString.h
TCLList.o: /usr/include/string.h
TCLList.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLList.o: /usr/include/malloc.h
TCLObject.o: TCLObject.h TCLInterpreterObject.h /usr/include/assert.h
TCLObject.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLObject.o: /usr/include/gnu/stubs.h TCLList.h TCLInterpreter.h TCLString.h
TCLObject.o: /usr/include/string.h
TCLObject.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLObject.o: TCLException.h TCLResult.h
TCLProcessor.o: TCLProcessor.h TCLInterpreter.h TCLString.h
TCLProcessor.o: /usr/include/string.h /usr/include/features.h
TCLProcessor.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLProcessor.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLProcessor.o: TCLResult.h TCLException.h TCLInterpreterObject.h
TCLProcessor.o: /usr/include/assert.h
TCLResult.o: TCLResult.h TCLString.h /usr/include/string.h
TCLResult.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLResult.o: /usr/include/gnu/stubs.h
TCLResult.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLResult.o: TCLInterpreter.h /usr/include/assert.h
TCLString.o: TCLString.h /usr/include/string.h /usr/include/features.h
TCLString.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLString.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLTest.o: TCLApplication.h TCLInterpreterObject.h /usr/include/assert.h
TCLTest.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLTest.o: /usr/include/gnu/stubs.h TCLVariable.h TCLProcessor.h
TCLTest.o: TCLInterpreter.h TCLString.h /usr/include/string.h
TCLTest.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h TCLResult.h
TCLTest.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
TCLTest.o: /usr/include/sys/types.h /usr/include/bits/types.h
TCLTest.o: /usr/include/time.h /usr/include/sys/stat.h
TCLTest.o: /usr/include/bits/stat.h
TCLTimer.o: TCLTimer.h TCLInterpreterObject.h /usr/include/assert.h
TCLTimer.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLTimer.o: /usr/include/gnu/stubs.h
TCLVariable.o: TCLVariable.h TCLInterpreter.h TCLString.h
TCLVariable.o: /usr/include/string.h /usr/include/features.h
TCLVariable.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLVariable.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TCLVariable.o: /usr/include/assert.h
