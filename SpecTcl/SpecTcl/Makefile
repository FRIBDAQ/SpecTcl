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

AppInit.o: TCLHistogrammer.h PseudoScript.h Globals.h RunControlPackage.h
AppInit.o: ParameterPackage.h SpectrumPackage.h DataSourcePackage.h
AppInit.o: GatePackage.h GateCommand.h GateFactory.h ApplyCommand.h
AppInit.o: UngateCommand.h TCLAnalyzer.h XamineEventHandler.h
AppInit.o: /usr/local/include/tk.h /usr/local/include/tcl.h
AppInit.o: /usr/include/stdio.h /usr/include/standards.h
AppInit.o: /usr/include/sys/seek.h /usr/include/va_list.h
AppInit.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
AppInit.o: /usr/include/sys/select.h /usr/include/getopt.h
AppInit.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
AppInit.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
AppInit.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
AppInit.o: /usr/local/include/tclDecls.h /usr/include/X11/Xlib.h
AppInit.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
AppInit.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
AppInit.o: /usr/local/include/tkDecls.h /usr/include/stdlib.h
AppInit.o: /usr/include/assert.h /usr/include/sys/sysinfo.h
ApplyCommand.o: ApplyCommand.h GatePackage.h GateCommand.h GateFactory.h
ApplyCommand.o: UngateCommand.h /usr/include/assert.h
ApplyCommand.o: /usr/include/standards.h /usr/include/string.h
ApplyCommand.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ApplyCommand.o: /usr/include/sys/select.h /usr/include/strings.h
AttachCommand.o: AttachCommand.h DataSourcePackage.h /usr/include/stdlib.h
AttachCommand.o: /usr/include/standards.h /usr/include/getopt.h
AttachCommand.o: /usr/include/sys/types.h
AttachCommand.o: /usr/include/mach/machine/vm_types.h
AttachCommand.o: /usr/include/sys/select.h /usr/include/assert.h
AttachCommand.o: /usr/include/string.h /usr/include/strings.h
BindCommand.o: BindCommand.h SpectrumPackage.h /usr/include/string.h
BindCommand.o: /usr/include/standards.h /usr/include/sys/types.h
BindCommand.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
BindCommand.o: /usr/include/strings.h
ChanCommand.o: ChanCommand.h SpectrumPackage.h /usr/local/include/tcl.h
ChanCommand.o: /usr/include/stdio.h /usr/include/standards.h
ChanCommand.o: /usr/include/sys/seek.h /usr/include/va_list.h
ChanCommand.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ChanCommand.o: /usr/include/sys/select.h /usr/include/getopt.h
ChanCommand.o: /usr/include/sys/limits.h
ChanCommand.o: /usr/include/sys/machine/machlimits.h
ChanCommand.o: /usr/include/sys/syslimits.h
ChanCommand.o: /usr/include/sys/machine/machtime.h
ChanCommand.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
ChanCommand.o: /usr/local/include/tclDecls.h /usr/include/string.h
ChanCommand.o: /usr/include/strings.h
ClearCommand.o: ClearCommand.h SpectrumPackage.h /usr/include/string.h
ClearCommand.o: /usr/include/standards.h /usr/include/sys/types.h
ClearCommand.o: /usr/include/mach/machine/vm_types.h
ClearCommand.o: /usr/include/sys/select.h /usr/include/strings.h
DataSourcePackage.o: DataSourcePackage.h AttachCommand.h TapeCommand.h
DataSourcePackage.o: Globals.h /usr/local/include/tcl.h /usr/include/stdio.h
DataSourcePackage.o: /usr/include/standards.h /usr/include/sys/seek.h
DataSourcePackage.o: /usr/include/va_list.h /usr/include/sys/types.h
DataSourcePackage.o: /usr/include/mach/machine/vm_types.h
DataSourcePackage.o: /usr/include/sys/select.h /usr/include/getopt.h
DataSourcePackage.o: /usr/include/sys/limits.h
DataSourcePackage.o: /usr/include/sys/machine/machlimits.h
DataSourcePackage.o: /usr/include/sys/syslimits.h
DataSourcePackage.o: /usr/include/sys/machine/machtime.h
DataSourcePackage.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
DataSourcePackage.o: /usr/local/include/tclDecls.h
EventProcessor.o: EventProcessor.h
GateCommand.o: GateCommand.h GateFactory.h GatePackage.h ApplyCommand.h
GateCommand.o: UngateCommand.h /usr/include/stdio.h /usr/include/standards.h
GateCommand.o: /usr/include/sys/seek.h /usr/include/va_list.h
GateCommand.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
GateCommand.o: /usr/include/sys/select.h /usr/include/getopt.h
GateCommand.o: /usr/include/sys/limits.h
GateCommand.o: /usr/include/sys/machine/machlimits.h
GateCommand.o: /usr/include/sys/syslimits.h
GateCommand.o: /usr/include/sys/machine/machtime.h
GateCommand.o: /usr/include/sys/rt_limits.h /usr/include/assert.h
GateCommand.o: /usr/include/string.h /usr/include/strings.h
GateFactory.o: GateFactory.h GateFactoryException.h /usr/include/assert.h
GateFactory.o: /usr/include/standards.h
GateFactoryException.o: GateFactoryException.h
GatePackage.o: GatePackage.h GateCommand.h GateFactory.h ApplyCommand.h
GatePackage.o: UngateCommand.h /usr/include/stdio.h /usr/include/standards.h
GatePackage.o: /usr/include/sys/seek.h /usr/include/va_list.h
GatePackage.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
GatePackage.o: /usr/include/sys/select.h /usr/include/getopt.h
GatePackage.o: /usr/include/sys/limits.h
GatePackage.o: /usr/include/sys/machine/machlimits.h
GatePackage.o: /usr/include/sys/syslimits.h
GatePackage.o: /usr/include/sys/machine/machtime.h
GatePackage.o: /usr/include/sys/rt_limits.h
MySpecTclApp.o: MySpecTclApp.h TclGrammerApp.h EventProcessor.h TCLAnalyzer.h
ParameterCommand.o: ParameterCommand.h ParameterPackage.h
ParameterCommand.o: /usr/local/include/tcl.h /usr/include/stdio.h
ParameterCommand.o: /usr/include/standards.h /usr/include/sys/seek.h
ParameterCommand.o: /usr/include/va_list.h /usr/include/sys/types.h
ParameterCommand.o: /usr/include/mach/machine/vm_types.h
ParameterCommand.o: /usr/include/sys/select.h /usr/include/getopt.h
ParameterCommand.o: /usr/include/sys/limits.h
ParameterCommand.o: /usr/include/sys/machine/machlimits.h
ParameterCommand.o: /usr/include/sys/syslimits.h
ParameterCommand.o: /usr/include/sys/machine/machtime.h
ParameterCommand.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
ParameterCommand.o: /usr/local/include/tclDecls.h /usr/include/string.h
ParameterCommand.o: /usr/include/strings.h /usr/include/malloc.h
ParameterPackage.o: ParameterPackage.h TCLHistogrammer.h PseudoScript.h
ParameterPackage.o: ParameterCommand.h PseudoCommand.h /usr/include/assert.h
ParameterPackage.o: /usr/include/standards.h /usr/include/stdio.h
ParameterPackage.o: /usr/include/sys/seek.h /usr/include/va_list.h
ParameterPackage.o: /usr/include/sys/types.h
ParameterPackage.o: /usr/include/mach/machine/vm_types.h
ParameterPackage.o: /usr/include/sys/select.h /usr/include/getopt.h
ParameterPackage.o: /usr/include/sys/limits.h
ParameterPackage.o: /usr/include/sys/machine/machlimits.h
ParameterPackage.o: /usr/include/sys/syslimits.h
ParameterPackage.o: /usr/include/sys/machine/machtime.h
ParameterPackage.o: /usr/include/sys/rt_limits.h
PseudoCommand.o: PseudoCommand.h ParameterPackage.h /usr/include/string.h
PseudoCommand.o: /usr/include/standards.h /usr/include/sys/types.h
PseudoCommand.o: /usr/include/mach/machine/vm_types.h
PseudoCommand.o: /usr/include/sys/select.h /usr/include/strings.h
PseudoScript.o: PseudoScript.h /usr/include/stdio.h /usr/include/standards.h
PseudoScript.o: /usr/include/sys/seek.h /usr/include/va_list.h
PseudoScript.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
PseudoScript.o: /usr/include/sys/select.h /usr/include/getopt.h
PseudoScript.o: /usr/include/sys/limits.h
PseudoScript.o: /usr/include/sys/machine/machlimits.h
PseudoScript.o: /usr/include/sys/syslimits.h
PseudoScript.o: /usr/include/sys/machine/machtime.h
PseudoScript.o: /usr/include/sys/rt_limits.h
ReadCommand.o: ReadCommand.h /usr/include/sys/stat.h /usr/include/standards.h
ReadCommand.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ReadCommand.o: /usr/include/sys/select.h /usr/include/sys/mode.h
ReadCommand.o: WriteCommand.h SpectrumPackage.h /usr/include/fcntl.h
ReadCommand.o: /usr/include/sys/fcntl.h /usr/include/sys/seek.h
ReadCommand.o: /usr/include/unistd.h /usr/include/sys/access.h
ReadCommand.o: /usr/include/getopt.h /usr/include/string.h
ReadCommand.o: /usr/include/strings.h /usr/include/errno.h
RunControlPackage.o: RunControlPackage.h StartRun.h StopRun.h Globals.h
SpectrumCommand.o: SpectrumCommand.h SpectrumPackage.h
SpectrumCommand.o: /usr/local/include/tcl.h /usr/include/stdio.h
SpectrumCommand.o: /usr/include/standards.h /usr/include/sys/seek.h
SpectrumCommand.o: /usr/include/va_list.h /usr/include/sys/types.h
SpectrumCommand.o: /usr/include/mach/machine/vm_types.h
SpectrumCommand.o: /usr/include/sys/select.h /usr/include/getopt.h
SpectrumCommand.o: /usr/include/sys/limits.h
SpectrumCommand.o: /usr/include/sys/machine/machlimits.h
SpectrumCommand.o: /usr/include/sys/syslimits.h
SpectrumCommand.o: /usr/include/sys/machine/machtime.h
SpectrumCommand.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
SpectrumCommand.o: /usr/local/include/tclDecls.h /usr/include/string.h
SpectrumCommand.o: /usr/include/strings.h /usr/include/malloc.h
SpectrumPackage.o: SpectrumPackage.h ClearCommand.h BindCommand.h
SpectrumPackage.o: UnbindCommand.h SpectrumCommand.h ChanCommand.h Globals.h
SpectrumPackage.o: WriteCommand.h ReadCommand.h /usr/include/sys/stat.h
SpectrumPackage.o: /usr/include/standards.h /usr/include/sys/types.h
SpectrumPackage.o: /usr/include/mach/machine/vm_types.h
SpectrumPackage.o: /usr/include/sys/select.h /usr/include/sys/mode.h
SpectrumPackage.o: /usr/local/include/tcl.h /usr/include/stdio.h
SpectrumPackage.o: /usr/include/sys/seek.h /usr/include/va_list.h
SpectrumPackage.o: /usr/include/getopt.h /usr/include/sys/limits.h
SpectrumPackage.o: /usr/include/sys/machine/machlimits.h
SpectrumPackage.o: /usr/include/sys/syslimits.h
SpectrumPackage.o: /usr/include/sys/machine/machtime.h
SpectrumPackage.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
SpectrumPackage.o: /usr/local/include/tclDecls.h /usr/include/assert.h
SpectrumPackage.o: /usr/include/math.h /usr/include/stdlib.h
SpectrumPackage.o: /usr/include/string.h /usr/include/strings.h
SpectrumPackage.o: /usr/include/errno.h /usr/include/limits.h
StartRun.o: StartRun.h RunControlPackage.h
StopRun.o: StopRun.h RunControlPackage.h
TCLAnalyzer.o: TCLAnalyzer.h EventProcessor.h
TCLHistogrammer.o: TCLHistogrammer.h PseudoScript.h /usr/include/assert.h
TCLHistogrammer.o: /usr/include/standards.h
TapeCommand.o: TapeCommand.h DataSourcePackage.h /usr/include/string.h
TapeCommand.o: /usr/include/standards.h /usr/include/sys/types.h
TapeCommand.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
TapeCommand.o: /usr/include/strings.h
TclGrammerApp.o: TclGrammerApp.h /usr/include/limits.h /usr/include/assert.h
TclGrammerApp.o: /usr/include/standards.h TCLHistogrammer.h PseudoScript.h
TclGrammerApp.o: Globals.h RunControlPackage.h ParameterPackage.h
TclGrammerApp.o: SpectrumPackage.h DataSourcePackage.h GatePackage.h
TclGrammerApp.o: GateCommand.h GateFactory.h ApplyCommand.h UngateCommand.h
TclGrammerApp.o: TCLAnalyzer.h XamineEventHandler.h /usr/local/include/tk.h
TclGrammerApp.o: /usr/local/include/tcl.h /usr/include/stdio.h
TclGrammerApp.o: /usr/include/sys/seek.h /usr/include/va_list.h
TclGrammerApp.o: /usr/include/sys/types.h
TclGrammerApp.o: /usr/include/mach/machine/vm_types.h
TclGrammerApp.o: /usr/include/sys/select.h /usr/include/getopt.h
TclGrammerApp.o: /usr/include/sys/limits.h
TclGrammerApp.o: /usr/include/sys/machine/machlimits.h
TclGrammerApp.o: /usr/include/sys/syslimits.h
TclGrammerApp.o: /usr/include/sys/machine/machtime.h
TclGrammerApp.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
TclGrammerApp.o: /usr/local/include/tclDecls.h /usr/include/X11/Xlib.h
TclGrammerApp.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TclGrammerApp.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
TclGrammerApp.o: /usr/local/include/tkDecls.h /usr/include/stdlib.h
TclGrammerApp.o: /usr/include/sys/sysinfo.h
UnbindCommand.o: UnbindCommand.h SpectrumPackage.h /usr/include/string.h
UnbindCommand.o: /usr/include/standards.h /usr/include/sys/types.h
UnbindCommand.o: /usr/include/mach/machine/vm_types.h
UnbindCommand.o: /usr/include/sys/select.h /usr/include/strings.h
UngateCommand.o: UngateCommand.h GatePackage.h GateCommand.h GateFactory.h
UngateCommand.o: ApplyCommand.h /usr/include/assert.h
UngateCommand.o: /usr/include/standards.h /usr/include/string.h
UngateCommand.o: /usr/include/sys/types.h
UngateCommand.o: /usr/include/mach/machine/vm_types.h
UngateCommand.o: /usr/include/sys/select.h /usr/include/strings.h
WriteCommand.o: WriteCommand.h SpectrumPackage.h /usr/include/errno.h
WriteCommand.o: /usr/include/standards.h /usr/include/string.h
WriteCommand.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
WriteCommand.o: /usr/include/sys/select.h /usr/include/strings.h
WriteCommand.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
WriteCommand.o: /usr/include/sys/seek.h /usr/include/sys/mode.h
WriteCommand.o: /usr/include/sys/stat.h /usr/include/stdio.h
WriteCommand.o: /usr/include/va_list.h /usr/include/getopt.h
WriteCommand.o: /usr/include/sys/limits.h
WriteCommand.o: /usr/include/sys/machine/machlimits.h
WriteCommand.o: /usr/include/sys/syslimits.h
WriteCommand.o: /usr/include/sys/machine/machtime.h
WriteCommand.o: /usr/include/sys/rt_limits.h /usr/include/unistd.h
WriteCommand.o: /usr/include/sys/access.h
XamineEventHandler.o: XamineEventHandler.h /usr/local/include/tk.h
XamineEventHandler.o: /usr/local/include/tcl.h /usr/include/stdio.h
XamineEventHandler.o: /usr/include/standards.h /usr/include/sys/seek.h
XamineEventHandler.o: /usr/include/va_list.h /usr/include/sys/types.h
XamineEventHandler.o: /usr/include/mach/machine/vm_types.h
XamineEventHandler.o: /usr/include/sys/select.h /usr/include/getopt.h
XamineEventHandler.o: /usr/include/sys/limits.h
XamineEventHandler.o: /usr/include/sys/machine/machlimits.h
XamineEventHandler.o: /usr/include/sys/syslimits.h
XamineEventHandler.o: /usr/include/sys/machine/machtime.h
XamineEventHandler.o: /usr/include/sys/rt_limits.h /usr/include/stdarg.h
XamineEventHandler.o: /usr/local/include/tclDecls.h /usr/include/X11/Xlib.h
XamineEventHandler.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
XamineEventHandler.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
XamineEventHandler.o: /usr/local/include/tkDecls.h GateFactory.h
XamineEventHandler.o: GatePackage.h GateCommand.h ApplyCommand.h
XamineEventHandler.o: UngateCommand.h /usr/include/assert.h
