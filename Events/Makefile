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

INCLUDEDIRS= $($(OS)INCS)-I. -I$(INSTDIR)/Include -I/usr/local/include



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

AnalysisTest.o: Event.h ./ValidValue.h EventList.h EventSink.h Analyzer.h
AnalysisTest.o: BufferDecoder.h ./BufferTranslator.h TranslatorPointer.h
AnalysisTest.o: NSCLBufferDecoder.h EventUnpacker.h RunControl.h
Analyzer.o: Analyzer.h BufferDecoder.h ./BufferTranslator.h
Analyzer.o: TranslatorPointer.h EventFormatError.h
BufferDecoder.o: BufferDecoder.h ./BufferTranslator.h TranslatorPointer.h
BufferDecoder.o: EventFormatError.h Analyzer.h
BufferTranslator.o: ./BufferTranslator.h TranslatorPointer.h
Event.o: Event.h ./ValidValue.h
EventFormatError.o: EventFormatError.h
EventList.o: EventList.h
EventSink.o: EventSink.h
NSCLBufferDecoder.o: NSCLBufferDecoder.h Analyzer.h BufferDecoder.h
NSCLBufferDecoder.o: ./BufferTranslator.h TranslatorPointer.h
NSCLBufferDecoder.o: EventFormatError.h /usr/include/assert.h
NSCLBufferDecoder.o: /usr/include/_ansi.h /usr/include/sys/config.h
NSCLBufferDecoder.o: /usr/include/stdlib.h /usr/include/sys/reent.h
NSCLBufferDecoder.o: /usr/include/sys/_types.h /usr/include/string.h
RunControl.o: RunControl.h /usr/include/assert.h /usr/include/_ansi.h
RunControl.o: /usr/include/sys/config.h
TCLRunFileHandler.o: TCLRunFileHandler.h RunControl.h TKRunControl.h
TCLRunFileHandler.o: /usr/include/tcl.h /usr/include/stdio.h
TCLRunFileHandler.o: /usr/include/_ansi.h /usr/include/sys/config.h
TCLRunFileHandler.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TCLRunFileHandler.o: /usr/include/tk.h /usr/local/include/X11/Xlib.h
TCLRunFileHandler.o: /usr/local/include/X11/X.h
TCLRunFileHandler.o: /usr/local/include/X11/Xfuncproto.h
TKRunControl.o: TKRunControl.h /usr/include/tcl.h /usr/include/stdio.h
TKRunControl.o: /usr/include/_ansi.h /usr/include/sys/config.h
TKRunControl.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TKRunControl.o: /usr/include/tk.h /usr/local/include/X11/Xlib.h
TKRunControl.o: /usr/local/include/X11/X.h
TKRunControl.o: /usr/local/include/X11/Xfuncproto.h
