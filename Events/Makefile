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
INSTALL=install -m 0664
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
	$(DEPEND) -Y. *.cpp

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

