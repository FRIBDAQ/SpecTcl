#  Makefile for the TCL Controlled  Histogramming program.
#  targets include:
#     system(default)             - Builds the software.
#     clean                       - Kills off .o, .a files.
#     depend                      - Rebuilds the dependencies with makedepend.
#                                   in current directory.
#
#
#

LinuxLIBS=-ldl
HPUXLIBDIR=-L/opt/tcltk/lib
HPUXINCS=-I/opt/tcltk/include

CYGWININCS=-I/usr/local/tcl/include
CYGWINLIBDIR=-L/usr/local/tcl/lib

INCLUDEDIRS= $($(OS)INCS) -I. -I$(INSTDIR)/Include -I/usr/local/include \
	-I/usr/local/daq/include 
LIBDIRS= $($(OS)LIBDIR) -L. -L$(INSTDIR)/Lib -L/usr/local \
	-L/usr/local/daq/lib -L/usr/X11/lib
LIBS=   -lTclGrammerCommands  \
	-lSorting -lXplus -lGates -lAnalysis -lEventSource -lSpecio \
	-ltclPlus -lException \
	-lXamine -ltape \
	-ltk -ltcl -lXt -lX11 -lm $($(OS)LIBS)

PROFILE=-g3 -pg
NOPROFILE=-g
ECHO=echo
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
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
	TclGrammerApp.o Globals.o	


OBJECTS=MySpecTclApp.o  

system: SpecTcl libTclGrammerCommands.a

clean: 
	$(RM) $(OBJECTS) $(LIBOBJS)
	$(RM) SpecTcl
	$(RM) libTclGrammerCommands.a


install: system
	$(INSTALL) libTclGrammerCommands.a $(INSTDIR)/Lib
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
	chmod u+w $(INSTDIR)/Skel/*         # Needed to ensure that
	chmod u+w $(INSTDIR)/Include/*      # stuff not checked out from
	chmod u+w $(INSTDIR)/Lib/*          # RCS can be overwritten on next
	chmod u+w $(INSTDIR)/Etc/*          # install.

depend:
	$(DEPEND) $(INCLUDEDIRS) *.cpp

SpecTcl: $(OBJECTS) libTclGrammerCommands.a
	$(LINK) -o SpecTcl $(OBJECTS) $(LIBS)

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

MySpecTclApp.o: MySpecTclApp.cpp
	$(COMPILE) MySpecTclApp.cpp

TclGrammerApp.o: TclGrammerApp.cpp
	$(COMPILE) TclGrammerApp.cpp


# DO NOT DELETE
