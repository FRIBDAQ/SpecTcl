#  Makefile for the TCL Controlled  Histogramming program.
#  targets include:
#     system(default)             - Builds the software.
#     clean                       - Kills off .o, .a files.
#     depend                      - Rebuilds the dependencies with makedepend.
#                                   in current directory.
#
#
#

SPECTCL_VERSION = -DVERSION=$(VERSION)
LinuxLIBS=-ldl
HPUXLIBDIR=-L/opt/tcltk/lib
HPUXINCS=-I/opt/tcltk/include
DarwinINCS = -I/usr/include -I/usr/X11R6/include \
	-I/usr/include/sys -I/usr/X11R6/include/X11

CYGWININCS=-I/usr/local/tcl/include
CYGWINLIBDIR=-L/usr/local/tcl/lib

INCLUDEDIRS= $($(OS)INCS) -I. -I$(INSTDIR)/Include -I/usr/local/include \
	-I/usr/local/daq/include 
LIBDIRS= $($(OS)LIBDIR) -L. -L$(INSTDIR)/Lib -L/usr/local \
	-L/usr/local/daq/lib -L/usr/X11/lib
LIBS=   -lTclGrammerCommands  \
	-lSorting -lXplus -lGates -lAnalysis -lEventSource -lSpecio \
	-ltclPlus -lException \
	-lXamine -ltape -ltape -lFilter \
	-ltk -ltcl -lXt -lX11 -lm $($(OS)LIBS)

PROFILE=-g3 -pg
NOPROFILE=-g
ECHO=echo
COMPILE=$($(OS)CPP) -c $(NOPROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"' $(SPECTCL_VERSION)
LINK=$($(OS)LD) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS) $(SPECTCL_VERSION)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
ARCHIVE=ar cr
CONCAT=cat

LIBOBJS=StopRun.o StartRun.o RunControlPackage.o \
	ParameterPackage.o ParameterCommand.o SpectrumPackage.o \
	ClearCommand.o BindCommand.o UnbindCommand.o SpectrumCommand.o \
	DataSourcePackage.o AttachCommand.o TapeCommand.o \
	GateCommand.o ApplyCommand.o UngateCommand.o GatePackage.o \
	GateFactory.o GateFactoryException.o XamineEventHandler.o \
	TCLHistogrammer.o PseudoCommand.o PseudoScript.o ChanCommand.o \
	WriteCommand.o ReadCommand.o TCLAnalyzer.o EventProcessor.o \
	TclGrammerApp.o Globals.o FilterCommand.o	


OBJECTS=MySpecTclApp.o  

system: SpecTcl libTclGrammerCommands.a

clean: 
	$(RM) $(OBJECTS) $(LIBOBJS)
	$(RM) SpecTcl
	$(RM) libTclGrammerCommands.a


install: system
	$(INSTALL) libTclGrammerCommands.a $(INSTDIR)/Lib
	ranlib $(INSTDIR)/Lib/libTclGrammerCommands.a
	$(INSTALL) MySpecTclApp.h          $(INSTDIR)/Skel
	$(INSTALL) MySpecTclApp.cpp	   $(INSTDIR)/Skel
	$(INSTALL) setup.tcl               $(INSTDIR)/Skel
	$(INSTALL) Globals.o                $(INSTDIR)/Lib
	$(INSTALL) *.h                      $(INSTDIR)/Include
	$(ECHO) INSTDIR=$(INSTDIR)          >$(INSTDIR)/Skel/Makefile
	$(ECHO) OS=$(OS)                    >>$(INSTDIR)/Skel/Makefile
	$(CONCAT) Makefile.skel            >>$(INSTDIR)/Skel/Makefile
	$(INSTALL) SpecTclInit.tcl          $(INSTDIR)/Etc
	$(INSTALL) SpecTclRC.tcl            $(INSTDIR)/Skel

depend:
	$(DEPEND) -Y.  *.cpp

SpecTcl: $(OBJECTS) libTclGrammerCommands.a
	ranlib $(INSTDIR)/Lib/*.a
	ranlib libTclGrammerCommands.a
	$(LINK)  -o SpecTcl $(OBJECTS) $(LIBS)

libTclGrammerCommands.a: $(LIBOBJS)
	$(ARCHIVE) libTclGrammerCommands.a $(LIBOBJS)

AppInit.o: AppInit.cpp
	$(COMPILE) AppInit.cpp

Globals.o: Globals.cpp
	$(COMPILE) Globals.cpp

UserCode.o: UserCode.cpp
	$(COMPILE) UserCode.cpp

# Library function builds.

StopRun.o: StopRun.cpp
	$(COMPILE) StopRun.cpp

StartRun.o: StartRun.cpp
	$(COMPILE) StartRun.cpp


RunControlPackage.o: RunControlPackage.cpp
	$(COMPILE) RunControlPackage.cpp

ParameterPackage.o: ParameterPackage.cpp
	$(COMPILE) ParameterPackage.cpp

ParameterCommand.o: ParameterCommand.cpp
	$(COMPILE) ParameterCommand.cpp

SpectrumPackage.o: SpectrumPackage.cpp
	$(COMPILE) SpectrumPackage.cpp

ClearCommand.o: ClearCommand.cpp
	$(COMPILE) ClearCommand.cpp

BindCommand.o: BindCommand.cpp
	$(COMPILE) BindCommand.cpp

UnbindCommand.o: UnbindCommand.cpp
	$(COMPILE) UnbindCommand.cpp

SpectrumCommand.o: SpectrumCommand.cpp
	$(COMPILE) SpectrumCommand.cpp

DataSourcePackage.o: DataSourcePackage.cpp
	$(COMPILE) DataSourcePackage.cpp

AttachCommand.o: AttachCommand.cpp
	$(COMPILE) AttachCommand.cpp

TapeCommand.o: TapeCommand.cpp
	$(COMPILE) TapeCommand.cpp


GateCommand.o: GateCommand.cpp
	$(COMPILE) GateCommand.cpp

ApplyCommand.o: ApplyCommand.cpp
	$(COMPILE) ApplyCommand.cpp

UngateCommand.o: UngateCommand.cpp
	$(COMPILE) UngateCommand.cpp

GatePackage.o: GatePackage.cpp
	$(COMPILE) GatePackage.cpp

GateFactory.o: GateFactory.cpp
	$(COMPILE) GateFactory.cpp


GateFactoryException.o: GateFactoryException.cpp
	$(COMPILE) GateFactoryException.cpp

XamineEventHandler.o: XamineEventHandler.cpp
	$(COMPILE) XamineEventHandler.cpp

TCLHistogrammer.o: TCLHistogrammer.cpp
	$(COMPILE) TCLHistogrammer.cpp


PseudoCommand.o: PseudoCommand.cpp
	$(COMPILE) PseudoCommand.cpp

PseudoScript.o: PseudoScript.cpp
	$(COMPILE) PseudoScript.cpp

ChanCommand.o: ChanCommand.cpp
	$(COMPILE) ChanCommand.cpp


WriteCommand.o: WriteCommand.cpp
	$(COMPILE) WriteCommand.cpp

ReadCommand.o: ReadCommand.cpp
	$(COMPILE) ReadCommand.cpp

TCLAnalyzer.o: TCLAnalyzer.cpp
	$(COMPILE) TCLAnalyzer.cpp

EventProcessor.o: EventProcessor.cpp
	$(COMPILE) EventProcessor.cpp

FilterCommand.o: FilterCommand.cpp
	$(COMPILE) FilterCommand.cpp

MySpecTclApp.o: MySpecTclApp.cpp
	$(COMPILE) MySpecTclApp.cpp

TclGrammerApp.o: TclGrammerApp.cpp
	$(COMPILE) TclGrammerApp.cpp
