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
AppInit.o: /usr/include/tk.h /usr/include/tcl.h /usr/include/stdio.h
AppInit.o: /usr/include/features.h /usr/include/sys/cdefs.h
AppInit.o: /usr/include/gnu/stubs.h
AppInit.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
AppInit.o: /usr/include/bits/types.h /usr/include/libio.h
AppInit.o: /usr/include/_G_config.h /usr/include/wchar.h
AppInit.o: /usr/include/bits/wchar.h /usr/include/gconv.h
AppInit.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
AppInit.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
AppInit.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
AppInit.o: /usr/include/time.h /usr/include/X11/X.h
AppInit.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
AppInit.o: /usr/include/tkDecls.h /usr/include/stdlib.h /usr/include/assert.h
AppInit.o: /usr/include/sys/sysinfo.h /usr/include/linux/kernel.h
ApplyCommand.o: ApplyCommand.h GatePackage.h GateCommand.h GateFactory.h
ApplyCommand.o: UngateCommand.h /usr/include/assert.h /usr/include/features.h
ApplyCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ApplyCommand.o: /usr/include/string.h
ApplyCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
AttachCommand.o: AttachCommand.h DataSourcePackage.h /usr/include/stdlib.h
AttachCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
AttachCommand.o: /usr/include/gnu/stubs.h
AttachCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
AttachCommand.o: /usr/include/assert.h /usr/include/string.h
BindCommand.o: BindCommand.h SpectrumPackage.h /usr/include/string.h
BindCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
BindCommand.o: /usr/include/gnu/stubs.h
BindCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
ChanCommand.o: ChanCommand.h SpectrumPackage.h /usr/include/tcl.h
ChanCommand.o: /usr/include/stdio.h /usr/include/features.h
ChanCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ChanCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
ChanCommand.o: /usr/include/bits/types.h /usr/include/libio.h
ChanCommand.o: /usr/include/_G_config.h /usr/include/wchar.h
ChanCommand.o: /usr/include/bits/wchar.h /usr/include/gconv.h
ChanCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
ChanCommand.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
ChanCommand.o: /usr/include/string.h
ClearCommand.o: ClearCommand.h SpectrumPackage.h /usr/include/string.h
ClearCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
ClearCommand.o: /usr/include/gnu/stubs.h
ClearCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
DataSourcePackage.o: DataSourcePackage.h AttachCommand.h TapeCommand.h
DataSourcePackage.o: Globals.h /usr/include/tcl.h /usr/include/stdio.h
DataSourcePackage.o: /usr/include/features.h /usr/include/sys/cdefs.h
DataSourcePackage.o: /usr/include/gnu/stubs.h
DataSourcePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
DataSourcePackage.o: /usr/include/bits/types.h /usr/include/libio.h
DataSourcePackage.o: /usr/include/_G_config.h /usr/include/wchar.h
DataSourcePackage.o: /usr/include/bits/wchar.h /usr/include/gconv.h
DataSourcePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
DataSourcePackage.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
EventProcessor.o: EventProcessor.h
GateCommand.o: GateCommand.h GateFactory.h GatePackage.h ApplyCommand.h
GateCommand.o: UngateCommand.h /usr/include/stdio.h /usr/include/features.h
GateCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GateCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
GateCommand.o: /usr/include/bits/types.h /usr/include/libio.h
GateCommand.o: /usr/include/_G_config.h /usr/include/wchar.h
GateCommand.o: /usr/include/bits/wchar.h /usr/include/gconv.h
GateCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
GateCommand.o: /usr/include/bits/stdio_lim.h /usr/include/assert.h
GateCommand.o: /usr/include/string.h
GateFactory.o: GateFactory.h GateFactoryException.h /usr/include/assert.h
GateFactory.o: /usr/include/features.h /usr/include/sys/cdefs.h
GateFactory.o: /usr/include/gnu/stubs.h
GateFactoryException.o: GateFactoryException.h
GatePackage.o: GatePackage.h GateCommand.h GateFactory.h ApplyCommand.h
GatePackage.o: UngateCommand.h /usr/include/stdio.h /usr/include/features.h
GatePackage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GatePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
GatePackage.o: /usr/include/bits/types.h /usr/include/libio.h
GatePackage.o: /usr/include/_G_config.h /usr/include/wchar.h
GatePackage.o: /usr/include/bits/wchar.h /usr/include/gconv.h
GatePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
GatePackage.o: /usr/include/bits/stdio_lim.h
MySpecTclApp.o: MySpecTclApp.h TclGrammerApp.h EventProcessor.h TCLAnalyzer.h
ParameterCommand.o: ParameterCommand.h ParameterPackage.h /usr/include/tcl.h
ParameterCommand.o: /usr/include/stdio.h /usr/include/features.h
ParameterCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ParameterCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
ParameterCommand.o: /usr/include/bits/types.h /usr/include/libio.h
ParameterCommand.o: /usr/include/_G_config.h /usr/include/wchar.h
ParameterCommand.o: /usr/include/bits/wchar.h /usr/include/gconv.h
ParameterCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
ParameterCommand.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
ParameterCommand.o: /usr/include/string.h /usr/include/malloc.h
ParameterPackage.o: ParameterPackage.h TCLHistogrammer.h ./PseudoScript.h
ParameterPackage.o: ParameterCommand.h PseudoCommand.h /usr/include/assert.h
ParameterPackage.o: /usr/include/features.h /usr/include/sys/cdefs.h
ParameterPackage.o: /usr/include/gnu/stubs.h /usr/include/stdio.h
ParameterPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
ParameterPackage.o: /usr/include/bits/types.h /usr/include/libio.h
ParameterPackage.o: /usr/include/_G_config.h /usr/include/wchar.h
ParameterPackage.o: /usr/include/bits/wchar.h /usr/include/gconv.h
ParameterPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
ParameterPackage.o: /usr/include/bits/stdio_lim.h
PseudoCommand.o: PseudoCommand.h ParameterPackage.h /usr/include/string.h
PseudoCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
PseudoCommand.o: /usr/include/gnu/stubs.h
PseudoCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
PseudoScript.o: ./PseudoScript.h /usr/include/stdio.h /usr/include/features.h
PseudoScript.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
PseudoScript.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
PseudoScript.o: /usr/include/bits/types.h /usr/include/libio.h
PseudoScript.o: /usr/include/_G_config.h /usr/include/wchar.h
PseudoScript.o: /usr/include/bits/wchar.h /usr/include/gconv.h
PseudoScript.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
PseudoScript.o: /usr/include/bits/stdio_lim.h
ReadCommand.o: ReadCommand.h /usr/include/sys/stat.h /usr/include/features.h
ReadCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ReadCommand.o: /usr/include/bits/types.h
ReadCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
ReadCommand.o: /usr/include/bits/stat.h /usr/include/sys/types.h
ReadCommand.o: /usr/include/time.h WriteCommand.h SpectrumPackage.h
ReadCommand.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
ReadCommand.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
ReadCommand.o: /usr/include/bits/confname.h /usr/include/string.h
ReadCommand.o: /usr/include/errno.h /usr/include/bits/errno.h
ReadCommand.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
RunControlPackage.o: RunControlPackage.h StartRun.h StopRun.h Globals.h
SpectrumCommand.o: SpectrumCommand.h SpectrumPackage.h /usr/include/tcl.h
SpectrumCommand.o: /usr/include/stdio.h /usr/include/features.h
SpectrumCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
SpectrumCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
SpectrumCommand.o: /usr/include/bits/types.h /usr/include/libio.h
SpectrumCommand.o: /usr/include/_G_config.h /usr/include/wchar.h
SpectrumCommand.o: /usr/include/bits/wchar.h /usr/include/gconv.h
SpectrumCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
SpectrumCommand.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
SpectrumCommand.o: /usr/include/string.h /usr/include/malloc.h
SpectrumPackage.o: SpectrumPackage.h ClearCommand.h BindCommand.h
SpectrumPackage.o: UnbindCommand.h SpectrumCommand.h ChanCommand.h Globals.h
SpectrumPackage.o: WriteCommand.h ReadCommand.h /usr/include/sys/stat.h
SpectrumPackage.o: /usr/include/features.h /usr/include/sys/cdefs.h
SpectrumPackage.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
SpectrumPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
SpectrumPackage.o: /usr/include/bits/stat.h /usr/include/sys/types.h
SpectrumPackage.o: /usr/include/time.h /usr/include/tcl.h
SpectrumPackage.o: /usr/include/stdio.h /usr/include/libio.h
SpectrumPackage.o: /usr/include/_G_config.h /usr/include/wchar.h
SpectrumPackage.o: /usr/include/bits/wchar.h /usr/include/gconv.h
SpectrumPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
SpectrumPackage.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
SpectrumPackage.o: /usr/include/assert.h /usr/include/math.h
SpectrumPackage.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
SpectrumPackage.o: /usr/include/bits/mathcalls.h /usr/include/string.h
SpectrumPackage.o: /usr/include/errno.h /usr/include/bits/errno.h
SpectrumPackage.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
SpectrumPackage.o: /usr/include/limits.h
SpectrumPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
StartRun.o: StartRun.h RunControlPackage.h
StopRun.o: StopRun.h RunControlPackage.h
TCLAnalyzer.o: TCLAnalyzer.h EventProcessor.h
TCLHistogrammer.o: TCLHistogrammer.h ./PseudoScript.h /usr/include/assert.h
TCLHistogrammer.o: /usr/include/features.h /usr/include/sys/cdefs.h
TCLHistogrammer.o: /usr/include/gnu/stubs.h
TapeCommand.o: TapeCommand.h DataSourcePackage.h /usr/include/string.h
TapeCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
TapeCommand.o: /usr/include/gnu/stubs.h
TapeCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TclGrammerApp.o: TclGrammerApp.h /usr/include/limits.h
TclGrammerApp.o: /usr/include/features.h /usr/include/sys/cdefs.h
TclGrammerApp.o: /usr/include/gnu/stubs.h
TclGrammerApp.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
TclGrammerApp.o: /usr/include/assert.h TCLHistogrammer.h ./PseudoScript.h
TclGrammerApp.o: Globals.h RunControlPackage.h ParameterPackage.h
TclGrammerApp.o: SpectrumPackage.h DataSourcePackage.h GatePackage.h
TclGrammerApp.o: GateCommand.h GateFactory.h ApplyCommand.h UngateCommand.h
TclGrammerApp.o: TCLAnalyzer.h XamineEventHandler.h /usr/include/tk.h
TclGrammerApp.o: /usr/include/tcl.h /usr/include/stdio.h
TclGrammerApp.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
TclGrammerApp.o: /usr/include/bits/types.h /usr/include/libio.h
TclGrammerApp.o: /usr/include/_G_config.h /usr/include/wchar.h
TclGrammerApp.o: /usr/include/bits/wchar.h /usr/include/gconv.h
TclGrammerApp.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
TclGrammerApp.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
TclGrammerApp.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
TclGrammerApp.o: /usr/include/time.h /usr/include/X11/X.h
TclGrammerApp.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TclGrammerApp.o: /usr/include/tkDecls.h /usr/include/stdlib.h
TclGrammerApp.o: /usr/include/sys/sysinfo.h /usr/include/linux/kernel.h
UnbindCommand.o: UnbindCommand.h SpectrumPackage.h /usr/include/string.h
UnbindCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
UnbindCommand.o: /usr/include/gnu/stubs.h
UnbindCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
UngateCommand.o: UngateCommand.h GatePackage.h GateCommand.h GateFactory.h
UngateCommand.o: ApplyCommand.h /usr/include/assert.h /usr/include/features.h
UngateCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
UngateCommand.o: /usr/include/string.h
UngateCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
WriteCommand.o: WriteCommand.h SpectrumPackage.h /usr/include/errno.h
WriteCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
WriteCommand.o: /usr/include/gnu/stubs.h /usr/include/bits/errno.h
WriteCommand.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
WriteCommand.o: /usr/include/string.h
WriteCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
WriteCommand.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
WriteCommand.o: /usr/include/sys/types.h /usr/include/bits/types.h
WriteCommand.o: /usr/include/time.h /usr/include/sys/stat.h
WriteCommand.o: /usr/include/bits/stat.h /usr/include/stdio.h
WriteCommand.o: /usr/include/libio.h /usr/include/_G_config.h
WriteCommand.o: /usr/include/wchar.h /usr/include/bits/wchar.h
WriteCommand.o: /usr/include/gconv.h
WriteCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
WriteCommand.o: /usr/include/bits/stdio_lim.h /usr/include/unistd.h
WriteCommand.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
XamineEventHandler.o: XamineEventHandler.h /usr/include/tk.h
XamineEventHandler.o: /usr/include/tcl.h /usr/include/stdio.h
XamineEventHandler.o: /usr/include/features.h /usr/include/sys/cdefs.h
XamineEventHandler.o: /usr/include/gnu/stubs.h
XamineEventHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
XamineEventHandler.o: /usr/include/bits/types.h /usr/include/libio.h
XamineEventHandler.o: /usr/include/_G_config.h /usr/include/wchar.h
XamineEventHandler.o: /usr/include/bits/wchar.h /usr/include/gconv.h
XamineEventHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
XamineEventHandler.o: /usr/include/bits/stdio_lim.h /usr/include/tclDecls.h
XamineEventHandler.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
XamineEventHandler.o: /usr/include/time.h /usr/include/X11/X.h
XamineEventHandler.o: /usr/include/X11/Xfuncproto.h
XamineEventHandler.o: /usr/include/X11/Xosdefs.h /usr/include/tkDecls.h
XamineEventHandler.o: GateFactory.h GatePackage.h GateCommand.h
XamineEventHandler.o: ApplyCommand.h UngateCommand.h /usr/include/assert.h
