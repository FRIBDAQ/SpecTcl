#
#  Makefile for event analysis subsystem.  
#  This makefile has the following targets:
#
#   system(default)        - Builds libAnalysis.a - the subsystem library.
#   install                - Installs the library and header files in the
#                            $(INSTDIR) directory tree.
#   depend                 - Re compute dependencies.
#   clean                  - Get rid of .o and .a files.
#   tests                  - Build a test program.
#


HPUXINCS=-I/opt/tcltk/include
HPUXLIBS=-L/opt/tcltk/lib
HPUXLIBDIR=-L/opt/tcltk/lib
DarwinINCS=-I/usr/X11R6/include -I/usr/X11R6/include/X11
DarwinLIBS=-L/usr/local/lib

INCLUDEDIRS= $($(OS)INCS) -I. -I$(INSTDIR)/Include -I/usr/local/include



LIBDIRS=-L/usr/local -L. -L$(INSTDIR)/Lib
LIBS=   $($(OS)LIBDIR) -lAnalysis -lEventSource -ltclPlus -lException \
	-ltk -ltcl -lXt -lX11 $($(OS)LIBS)

ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr

OBJECTS=Analyzer.o BufferDecoder.o BufferTranslator.o Event.o \
	EventFormatError.o EventList.o NSCLBufferDecoder.o RunControl.o \
	TCLRunFileHandler.o TKRunControl.o EventSink.o

system: libAnalysis.a
	$(ECHO) libAnalysis.a library rebuilt.

install: system
	$(INSTALL) libAnalysis.a $(INSTDIR)/Lib
	$(INSTALL) *.h           $(INSTDIR)/Include

clean:
	$(RM) libAnalysis.a
	$(RM) $(OBJECTS)

tests: AnalysisTest
	$(ECHO) Test rebuilt.

depend: 
	$(DEPEND) $(INCLUDEDIRS) *.cpp

libAnalysis.a: $(OBJECTS)
	$(ARCHIVE) libAnalysis.a $(OBJECTS)


AnalysisTest:  system AnalysisTest.cpp
	$(LINK) -o AnalysisTest AnalysisTest.cpp $(LIBS)

Analyzer.o: Analyzer.cpp
	$(COMPILE) Analyzer.cpp

BufferDecoder.o: BufferDecoder.cpp
	$(COMPILE) BufferDecoder.cpp

BufferTranslator.o: BufferTranslator.cpp BufferTranslator.h \
                    TranslatorPointer.h
	$(COMPILE)  BufferTranslator.cpp

Event.o: Event.cpp
	$(COMPILE) Event.cpp

EventFormatError.o: EventFormatError.cpp
	$(COMPILE) EventFormatError.cpp

EventList.o: EventList.cpp
	$(COMPILE) EventList.cpp

EventSink.o: EventSink.cpp
	$(COMPILE) EventSink.cpp

NSCLBufferDecoder.o: NSCLBufferDecoder.cpp
	$(COMPILE) NSCLBufferDecoder.cpp

RunControl.o: RunControl.cpp
	$(COMPILE) RunControl.cpp

TCLRunFileHandler.o: TCLRunFileHandler.cpp
	$(COMPILE) TCLRunFileHandler.cpp

TKRunControl.o: TKRunControl.cpp
	$(COMPILE) TKRunControl.cpp

# DO NOT DELETE

AnalysisTest.o: /Include/histotypes.h /usr/include/string.h
AnalysisTest.o: /usr/include/features.h /usr/include/sys/cdefs.h
AnalysisTest.o: /usr/include/gnu/stubs.h
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
AnalysisTest.o: /usr/include/math.h /usr/include/bits/huge_val.h
AnalysisTest.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
AnalysisTest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h Event.h
AnalysisTest.o: ./ValidValue.h EventList.h EventSink.h Analyzer.h
AnalysisTest.o: BufferDecoder.h ./BufferTranslator.h /Include/buffer.h
AnalysisTest.o: /Include/daqtypes.h TranslatorPointer.h NSCLBufferDecoder.h
AnalysisTest.o: EventUnpacker.h RunControl.h
Analyzer.o: Analyzer.h BufferDecoder.h ./BufferTranslator.h /Include/buffer.h
Analyzer.o: /Include/daqtypes.h /Include/histotypes.h /usr/include/string.h
Analyzer.o: /usr/include/features.h /usr/include/sys/cdefs.h
Analyzer.o: /usr/include/gnu/stubs.h
Analyzer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Analyzer.o: /usr/include/math.h /usr/include/bits/huge_val.h
Analyzer.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Analyzer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
Analyzer.o: TranslatorPointer.h EventFormatError.h
BufferDecoder.o: BufferDecoder.h ./BufferTranslator.h /Include/buffer.h
BufferDecoder.o: /Include/daqtypes.h /Include/histotypes.h
BufferDecoder.o: /usr/include/string.h /usr/include/features.h
BufferDecoder.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
BufferDecoder.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
BufferDecoder.o: /usr/include/math.h /usr/include/bits/huge_val.h
BufferDecoder.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
BufferDecoder.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
BufferDecoder.o: TranslatorPointer.h EventFormatError.h Analyzer.h
BufferDecoder.o: /Include/buftypes.h
BufferTranslator.o: /Include/buffer.h /Include/daqtypes.h
BufferTranslator.o: /Include/histotypes.h /usr/include/string.h
BufferTranslator.o: /usr/include/features.h /usr/include/sys/cdefs.h
BufferTranslator.o: /usr/include/gnu/stubs.h
BufferTranslator.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
BufferTranslator.o: /usr/include/math.h /usr/include/bits/huge_val.h
BufferTranslator.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
BufferTranslator.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
BufferTranslator.o: ./BufferTranslator.h TranslatorPointer.h
Event.o: Event.h ./ValidValue.h /Include/histotypes.h /usr/include/string.h
Event.o: /usr/include/features.h /usr/include/sys/cdefs.h
Event.o: /usr/include/gnu/stubs.h
Event.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Event.o: /usr/include/math.h /usr/include/bits/huge_val.h
Event.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Event.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
EventFormatError.o: EventFormatError.h
EventList.o: EventList.h /Include/histotypes.h /usr/include/string.h
EventList.o: /usr/include/features.h /usr/include/sys/cdefs.h
EventList.o: /usr/include/gnu/stubs.h
EventList.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
EventList.o: /usr/include/math.h /usr/include/bits/huge_val.h
EventList.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
EventList.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
EventSink.o: EventSink.h
NSCLBufferDecoder.o: NSCLBufferDecoder.h Analyzer.h BufferDecoder.h
NSCLBufferDecoder.o: ./BufferTranslator.h /Include/buffer.h
NSCLBufferDecoder.o: /Include/daqtypes.h /Include/histotypes.h
NSCLBufferDecoder.o: /usr/include/string.h /usr/include/features.h
NSCLBufferDecoder.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
NSCLBufferDecoder.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
NSCLBufferDecoder.o: /usr/include/math.h /usr/include/bits/huge_val.h
NSCLBufferDecoder.o: /usr/include/bits/mathdef.h
NSCLBufferDecoder.o: /usr/include/bits/mathcalls.h
NSCLBufferDecoder.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
NSCLBufferDecoder.o: TranslatorPointer.h EventFormatError.h
NSCLBufferDecoder.o: /Include/buftypes.h /usr/include/assert.h
NSCLBufferDecoder.o: /usr/include/stdlib.h /usr/include/sys/types.h
NSCLBufferDecoder.o: /usr/include/bits/types.h /usr/include/time.h
NSCLBufferDecoder.o: /usr/include/endian.h /usr/include/bits/endian.h
NSCLBufferDecoder.o: /usr/include/sys/select.h /usr/include/bits/select.h
NSCLBufferDecoder.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
NSCLBufferDecoder.o: /usr/include/alloca.h
RunControl.o: RunControl.h /usr/include/assert.h /usr/include/features.h
RunControl.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLRunFileHandler.o: TCLRunFileHandler.h RunControl.h /Include/histotypes.h
TCLRunFileHandler.o: /usr/include/string.h /usr/include/features.h
TCLRunFileHandler.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLRunFileHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
TCLRunFileHandler.o: /usr/include/math.h /usr/include/bits/huge_val.h
TCLRunFileHandler.o: /usr/include/bits/mathdef.h
TCLRunFileHandler.o: /usr/include/bits/mathcalls.h
TCLRunFileHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
TCLRunFileHandler.o: TKRunControl.h /usr/include/tcl.h /usr/include/stdio.h
TCLRunFileHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
TCLRunFileHandler.o: /usr/include/bits/types.h /usr/include/libio.h
TCLRunFileHandler.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLRunFileHandler.o: /usr/include/tk.h /usr/include/X11/Xlib.h
TCLRunFileHandler.o: /usr/include/sys/types.h /usr/include/time.h
TCLRunFileHandler.o: /usr/include/endian.h /usr/include/bits/endian.h
TCLRunFileHandler.o: /usr/include/sys/select.h /usr/include/bits/select.h
TCLRunFileHandler.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
TCLRunFileHandler.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TCLRunFileHandler.o: /usr/include/X11/Xosdefs.h
TKRunControl.o: TKRunControl.h /usr/include/tcl.h /usr/include/stdio.h
TKRunControl.o: /usr/include/features.h /usr/include/sys/cdefs.h
TKRunControl.o: /usr/include/gnu/stubs.h
TKRunControl.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
TKRunControl.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
TKRunControl.o: /usr/include/bits/types.h /usr/include/libio.h
TKRunControl.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TKRunControl.o: /usr/include/tk.h /usr/include/X11/Xlib.h
TKRunControl.o: /usr/include/sys/types.h /usr/include/time.h
TKRunControl.o: /usr/include/endian.h /usr/include/bits/endian.h
TKRunControl.o: /usr/include/sys/select.h /usr/include/bits/select.h
TKRunControl.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
TKRunControl.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TKRunControl.o: /usr/include/X11/Xosdefs.h
