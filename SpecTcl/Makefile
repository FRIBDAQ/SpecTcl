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
	-lXamine -ltape -ltape \
	-ltk -ltcl -lXt -lX11 -lm $($(OS)LIBS)

PROFILE=-g3 -pg
NOPROFILE=-g
ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
LINK=$($(OS)LD) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
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
	chmod u+w $(INSTDIR)/Skel/*         # Needed to ensure that
	chmod u+w $(INSTDIR)/Include/*      # stuff not checked out from
	chmod u+w $(INSTDIR)/Lib/*          # RCS can be overwritten on next
	chmod u+w $(INSTDIR)/Etc/*          # install.

depend:
	$(DEPEND) $(INCLUDEDIRS) *.cpp

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

MySpecTclApp.o: MySpecTclApp.cpp
	$(COMPILE) MySpecTclApp.cpp

TclGrammerApp.o: TclGrammerApp.cpp
	$(COMPILE) TclGrammerApp.cpp


# DO NOT DELETE

AppInit.o: TCLHistogrammer.h ./PseudoScript.h Globals.h RunControlPackage.h
AppInit.o: ParameterPackage.h SpectrumPackage.h DataSourcePackage.h
AppInit.o: GatePackage.h GateCommand.h GateFactory.h ApplyCommand.h
AppInit.o: UngateCommand.h TCLAnalyzer.h XamineEventHandler.h
AppInit.o: /usr/local/tcl/include/tk.h /usr/local/tcl/include/tcl.h
AppInit.o: /usr/include/stdio.h /usr/include/_ansi.h
AppInit.o: /usr/include/sys/config.h /usr/include/sys/reent.h
AppInit.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
AppInit.o: /usr/local/tcl/include/X11/Xlib.h /usr/local/tcl/include/X11/X.h
AppInit.o: /usr/local/tcl/include/X11/Xfuncproto.h
AppInit.o: /usr/local/tcl/include/tkIntXlibDecls.h
AppInit.o: /usr/local/tcl/include/X11/Xutil.h
AppInit.o: /usr/local/tcl/include/tkDecls.h /usr/include/stdlib.h
AppInit.o: /usr/include/assert.h
ApplyCommand.o: ApplyCommand.h GatePackage.h GateCommand.h GateFactory.h
ApplyCommand.o: UngateCommand.h /usr/include/assert.h /usr/include/_ansi.h
ApplyCommand.o: /usr/include/sys/config.h /usr/include/string.h
ApplyCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
AttachCommand.o: AttachCommand.h DataSourcePackage.h /usr/include/stdlib.h
AttachCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
AttachCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
AttachCommand.o: /usr/include/assert.h /usr/include/string.h
BindCommand.o: BindCommand.h SpectrumPackage.h /usr/include/string.h
BindCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
BindCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
ChanCommand.o: ChanCommand.h SpectrumPackage.h /usr/local/tcl/include/tcl.h
ChanCommand.o: /usr/include/stdio.h /usr/include/_ansi.h
ChanCommand.o: /usr/include/sys/config.h /usr/include/sys/reent.h
ChanCommand.o: /usr/include/sys/_types.h /usr/local/tcl/include/tclDecls.h
ChanCommand.o: /usr/include/string.h
ClearCommand.o: ClearCommand.h SpectrumPackage.h /usr/include/string.h
ClearCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
ClearCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
DataSourcePackage.o: DataSourcePackage.h AttachCommand.h TapeCommand.h
DataSourcePackage.o: Globals.h /usr/local/tcl/include/tcl.h
DataSourcePackage.o: /usr/include/stdio.h /usr/include/_ansi.h
DataSourcePackage.o: /usr/include/sys/config.h /usr/include/sys/reent.h
DataSourcePackage.o: /usr/include/sys/_types.h
DataSourcePackage.o: /usr/local/tcl/include/tclDecls.h
EventProcessor.o: EventProcessor.h
GateCommand.o: GateCommand.h GateFactory.h GatePackage.h ApplyCommand.h
GateCommand.o: UngateCommand.h /usr/include/stdio.h /usr/include/_ansi.h
GateCommand.o: /usr/include/sys/config.h /usr/include/sys/reent.h
GateCommand.o: /usr/include/sys/_types.h /usr/include/assert.h
GateCommand.o: /usr/include/string.h
GateFactory.o: GateFactory.h GateFactoryException.h /usr/include/assert.h
GateFactory.o: /usr/include/_ansi.h /usr/include/sys/config.h
GateFactoryException.o: GateFactoryException.h
GatePackage.o: GatePackage.h GateCommand.h GateFactory.h ApplyCommand.h
GatePackage.o: UngateCommand.h /usr/include/stdio.h /usr/include/_ansi.h
GatePackage.o: /usr/include/sys/config.h /usr/include/sys/reent.h
GatePackage.o: /usr/include/sys/_types.h
MySpecTclApp.o: MySpecTclApp.h TclGrammerApp.h EventProcessor.h TCLAnalyzer.h
ParameterCommand.o: ParameterCommand.h ParameterPackage.h
ParameterCommand.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
ParameterCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
ParameterCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
ParameterCommand.o: /usr/local/tcl/include/tclDecls.h /usr/include/string.h
ParameterCommand.o: /usr/include/malloc.h
ParameterPackage.o: ParameterPackage.h TCLHistogrammer.h ./PseudoScript.h
ParameterPackage.o: ParameterCommand.h PseudoCommand.h /usr/include/assert.h
ParameterPackage.o: /usr/include/_ansi.h /usr/include/sys/config.h
ParameterPackage.o: /usr/include/stdio.h /usr/include/sys/reent.h
ParameterPackage.o: /usr/include/sys/_types.h
PseudoCommand.o: PseudoCommand.h ParameterPackage.h /usr/include/string.h
PseudoCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
PseudoCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
PseudoScript.o: ./PseudoScript.h /usr/include/stdio.h /usr/include/_ansi.h
PseudoScript.o: /usr/include/sys/config.h /usr/include/sys/reent.h
PseudoScript.o: /usr/include/sys/_types.h
ReadCommand.o: ReadCommand.h /usr/include/sys/stat.h /usr/include/_ansi.h
ReadCommand.o: /usr/include/sys/config.h /usr/include/time.h
ReadCommand.o: /usr/include/machine/time.h /usr/include/sys/types.h
ReadCommand.o: /usr/include/machine/types.h /usr/include/sys/features.h
ReadCommand.o: WriteCommand.h SpectrumPackage.h /usr/include/fcntl.h
ReadCommand.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
ReadCommand.o: /usr/include/sys/unistd.h /usr/include/sys/_types.h
ReadCommand.o: /usr/include/getopt.h /usr/include/string.h
ReadCommand.o: /usr/include/sys/reent.h /usr/include/errno.h
ReadCommand.o: /usr/include/sys/errno.h
RunControlPackage.o: RunControlPackage.h StartRun.h StopRun.h Globals.h
SpectrumCommand.o: SpectrumCommand.h SpectrumPackage.h
SpectrumCommand.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
SpectrumCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
SpectrumCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
SpectrumCommand.o: /usr/local/tcl/include/tclDecls.h /usr/include/string.h
SpectrumCommand.o: /usr/include/malloc.h
SpectrumPackage.o: SpectrumPackage.h ClearCommand.h BindCommand.h
SpectrumPackage.o: UnbindCommand.h SpectrumCommand.h ChanCommand.h Globals.h
SpectrumPackage.o: WriteCommand.h ReadCommand.h /usr/include/sys/stat.h
SpectrumPackage.o: /usr/include/_ansi.h /usr/include/sys/config.h
SpectrumPackage.o: /usr/include/time.h /usr/include/machine/time.h
SpectrumPackage.o: /usr/include/sys/types.h /usr/include/machine/types.h
SpectrumPackage.o: /usr/include/sys/features.h /usr/local/tcl/include/tcl.h
SpectrumPackage.o: /usr/include/stdio.h /usr/include/sys/reent.h
SpectrumPackage.o: /usr/include/sys/_types.h
SpectrumPackage.o: /usr/local/tcl/include/tclDecls.h /usr/include/assert.h
SpectrumPackage.o: /usr/include/math.h /usr/include/machine/ieeefp.h
SpectrumPackage.o: /usr/include/string.h /usr/include/errno.h
SpectrumPackage.o: /usr/include/sys/errno.h /usr/include/limits.h
StartRun.o: StartRun.h RunControlPackage.h
StopRun.o: StopRun.h RunControlPackage.h
TCLAnalyzer.o: TCLAnalyzer.h EventProcessor.h
TCLHistogrammer.o: TCLHistogrammer.h ./PseudoScript.h /usr/include/assert.h
TCLHistogrammer.o: /usr/include/_ansi.h /usr/include/sys/config.h
TapeCommand.o: TapeCommand.h DataSourcePackage.h /usr/include/string.h
TapeCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
TapeCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TclGrammerApp.o: TclGrammerApp.h /usr/include/limits.h /usr/include/assert.h
TclGrammerApp.o: /usr/include/_ansi.h /usr/include/sys/config.h
TclGrammerApp.o: TCLHistogrammer.h ./PseudoScript.h Globals.h
TclGrammerApp.o: RunControlPackage.h ParameterPackage.h SpectrumPackage.h
TclGrammerApp.o: DataSourcePackage.h GatePackage.h GateCommand.h
TclGrammerApp.o: GateFactory.h ApplyCommand.h UngateCommand.h TCLAnalyzer.h
TclGrammerApp.o: XamineEventHandler.h /usr/local/tcl/include/tk.h
TclGrammerApp.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
TclGrammerApp.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
TclGrammerApp.o: /usr/local/tcl/include/tclDecls.h
TclGrammerApp.o: /usr/local/tcl/include/X11/Xlib.h
TclGrammerApp.o: /usr/local/tcl/include/X11/X.h
TclGrammerApp.o: /usr/local/tcl/include/X11/Xfuncproto.h
TclGrammerApp.o: /usr/local/tcl/include/tkIntXlibDecls.h
TclGrammerApp.o: /usr/local/tcl/include/X11/Xutil.h
TclGrammerApp.o: /usr/local/tcl/include/tkDecls.h /usr/include/stdlib.h
UnbindCommand.o: UnbindCommand.h SpectrumPackage.h /usr/include/string.h
UnbindCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
UnbindCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
UngateCommand.o: UngateCommand.h GatePackage.h GateCommand.h GateFactory.h
UngateCommand.o: ApplyCommand.h /usr/include/assert.h /usr/include/_ansi.h
UngateCommand.o: /usr/include/sys/config.h /usr/include/string.h
UngateCommand.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
WriteCommand.o: WriteCommand.h SpectrumPackage.h /usr/include/errno.h
WriteCommand.o: /usr/include/sys/errno.h /usr/include/sys/reent.h
WriteCommand.o: /usr/include/_ansi.h /usr/include/sys/config.h
WriteCommand.o: /usr/include/sys/_types.h /usr/include/string.h
WriteCommand.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
WriteCommand.o: /usr/include/sys/types.h /usr/include/machine/types.h
WriteCommand.o: /usr/include/sys/stat.h /usr/include/time.h
WriteCommand.o: /usr/include/machine/time.h /usr/include/sys/features.h
WriteCommand.o: /usr/include/stdio.h /usr/include/unistd.h
WriteCommand.o: /usr/include/sys/unistd.h /usr/include/getopt.h
XamineEventHandler.o: XamineEventHandler.h /usr/local/tcl/include/tk.h
XamineEventHandler.o: /usr/local/tcl/include/tcl.h /usr/include/stdio.h
XamineEventHandler.o: /usr/include/_ansi.h /usr/include/sys/config.h
XamineEventHandler.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
XamineEventHandler.o: /usr/local/tcl/include/tclDecls.h
XamineEventHandler.o: /usr/local/tcl/include/X11/Xlib.h
XamineEventHandler.o: /usr/local/tcl/include/X11/X.h
XamineEventHandler.o: /usr/local/tcl/include/X11/Xfuncproto.h
XamineEventHandler.o: /usr/local/tcl/include/tkIntXlibDecls.h
XamineEventHandler.o: /usr/local/tcl/include/X11/Xutil.h
XamineEventHandler.o: /usr/local/tcl/include/tkDecls.h GateFactory.h
XamineEventHandler.o: GatePackage.h GateCommand.h ApplyCommand.h
XamineEventHandler.o: UngateCommand.h /usr/include/assert.h
