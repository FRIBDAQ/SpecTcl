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

AppInit.o: TCLHistogrammer.h ./PseudoScript.h /Include/TCLInterpreterObject.h
AppInit.o: /usr/local/include/tcl.h /usr/include/stdio.h
AppInit.o: /usr/include/features.h /usr/include/sys/cdefs.h
AppInit.o: /usr/include/gnu/stubs.h
AppInit.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
AppInit.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
AppInit.o: /usr/include/bits/types.h /usr/include/libio.h
AppInit.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
AppInit.o: /usr/local/include/tclDecls.h /Include/histotypes.h
AppInit.o: /usr/include/string.h /usr/include/math.h
AppInit.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
AppInit.o: /usr/include/bits/mathcalls.h
AppInit.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
AppInit.o: /usr/include/assert.h /Include/TCLObject.h /Include/TCLList.h
AppInit.o: /Include/TCLApplication.h /Include/TCLVariable.h
AppInit.o: /Include/TCLProcessor.h /Include/TCLInterpreter.h
AppInit.o: /Include/TCLString.h /Include/TCLResult.h Globals.h
AppInit.o: RunControlPackage.h /Include/TCLCommandPackage.h
AppInit.o: ParameterPackage.h SpectrumPackage.h DataSourcePackage.h
AppInit.o: GatePackage.h GateCommand.h /Include/TCLPackagedCommand.h
AppInit.o: GateFactory.h ApplyCommand.h UngateCommand.h TCLAnalyzer.h
AppInit.o: XamineEventHandler.h /Include/TCLFileHandler.h
AppInit.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
AppInit.o: /usr/include/bits/confname.h /usr/include/getopt.h
AppInit.o: /usr/local/include/tk.h /usr/include/X11/Xlib.h
AppInit.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
AppInit.o: /usr/include/bits/endian.h /usr/include/sys/select.h
AppInit.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
AppInit.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
AppInit.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
AppInit.o: /usr/local/include/tkDecls.h /usr/include/stdlib.h
AppInit.o: /usr/include/alloca.h /usr/include/sys/sysinfo.h
AppInit.o: /usr/include/linux/kernel.h
ApplyCommand.o: ApplyCommand.h GatePackage.h GateCommand.h
ApplyCommand.o: /Include/TCLPackagedCommand.h GateFactory.h UngateCommand.h
ApplyCommand.o: /Include/TCLInterpreter.h /Include/TCLString.h
ApplyCommand.o: /usr/include/string.h /usr/include/features.h
ApplyCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ApplyCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ApplyCommand.o: /Include/TCLResult.h /usr/include/assert.h
AttachCommand.o: AttachCommand.h /Include/TCLPackagedCommand.h
AttachCommand.o: /Include/TCLInterpreter.h /Include/TCLString.h
AttachCommand.o: /usr/include/string.h /usr/include/features.h
AttachCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
AttachCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
AttachCommand.o: /Include/TCLCommandPackage.h /Include/TCLResult.h
AttachCommand.o: DataSourcePackage.h /Include/histotypes.h
AttachCommand.o: /usr/include/math.h /usr/include/bits/huge_val.h
AttachCommand.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
AttachCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
AttachCommand.o: /usr/include/stdlib.h /usr/include/sys/types.h
AttachCommand.o: /usr/include/bits/types.h /usr/include/time.h
AttachCommand.o: /usr/include/endian.h /usr/include/bits/endian.h
AttachCommand.o: /usr/include/sys/select.h /usr/include/bits/select.h
AttachCommand.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
AttachCommand.o: /usr/include/alloca.h /usr/include/assert.h
BindCommand.o: BindCommand.h /Include/TCLPackagedCommand.h
BindCommand.o: /Include/TCLString.h /usr/include/string.h
BindCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
BindCommand.o: /usr/include/gnu/stubs.h
BindCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
BindCommand.o: SpectrumPackage.h /Include/histotypes.h /usr/include/math.h
BindCommand.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
BindCommand.o: /usr/include/bits/mathcalls.h
BindCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
ChanCommand.o: ChanCommand.h /Include/TCLPackagedCommand.h SpectrumPackage.h
ChanCommand.o: /Include/TCLList.h /Include/TCLObject.h
ChanCommand.o: /usr/local/include/tcl.h /usr/include/stdio.h
ChanCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
ChanCommand.o: /usr/include/gnu/stubs.h
ChanCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ChanCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
ChanCommand.o: /usr/include/bits/types.h /usr/include/libio.h
ChanCommand.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
ChanCommand.o: /usr/local/include/tclDecls.h /Include/TCLException.h
ChanCommand.o: /Include/TCLInterpreterObject.h /Include/histotypes.h
ChanCommand.o: /usr/include/string.h /usr/include/math.h
ChanCommand.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
ChanCommand.o: /usr/include/bits/mathcalls.h
ChanCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
ChanCommand.o: /usr/include/assert.h /Include/TCLResult.h
ChanCommand.o: /Include/CException.h
ClearCommand.o: ClearCommand.h /Include/TCLPackagedCommand.h
ClearCommand.o: SpectrumPackage.h /Include/TCLInterpreter.h
ClearCommand.o: /Include/TCLString.h /usr/include/string.h
ClearCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
ClearCommand.o: /usr/include/gnu/stubs.h
ClearCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ClearCommand.o: /Include/TCLResult.h
DataSourcePackage.o: DataSourcePackage.h AttachCommand.h
DataSourcePackage.o: /Include/TCLPackagedCommand.h TapeCommand.h
DataSourcePackage.o: /Include/TCLInterpreter.h /Include/TCLString.h
DataSourcePackage.o: /usr/include/string.h /usr/include/features.h
DataSourcePackage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
DataSourcePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
DataSourcePackage.o: /Include/TCLResult.h Globals.h /usr/local/include/tcl.h
DataSourcePackage.o: /usr/include/stdio.h
DataSourcePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
DataSourcePackage.o: /usr/include/bits/types.h /usr/include/libio.h
DataSourcePackage.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
DataSourcePackage.o: /usr/local/include/tclDecls.h
EventProcessor.o: EventProcessor.h /Include/histotypes.h
EventProcessor.o: /usr/include/string.h /usr/include/features.h
EventProcessor.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
EventProcessor.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
EventProcessor.o: /usr/include/math.h /usr/include/bits/huge_val.h
EventProcessor.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
EventProcessor.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
GateCommand.o: GateCommand.h /Include/TCLPackagedCommand.h GateFactory.h
GateCommand.o: GatePackage.h ApplyCommand.h UngateCommand.h
GateCommand.o: /Include/TCLList.h /Include/TCLString.h /usr/include/string.h
GateCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
GateCommand.o: /usr/include/gnu/stubs.h
GateCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
GateCommand.o: /Include/TCLResult.h /usr/include/stdio.h
GateCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
GateCommand.o: /usr/include/bits/types.h /usr/include/libio.h
GateCommand.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GateCommand.o: /usr/include/assert.h
GateFactory.o: GateFactory.h GateFactoryException.h /Include/histotypes.h
GateFactory.o: /usr/include/string.h /usr/include/features.h
GateFactory.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GateFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
GateFactory.o: /usr/include/math.h /usr/include/bits/huge_val.h
GateFactory.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
GateFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
GateFactory.o: /usr/include/assert.h
GateFactoryException.o: GateFactoryException.h
GatePackage.o: GatePackage.h GateCommand.h /Include/TCLPackagedCommand.h
GatePackage.o: GateFactory.h ApplyCommand.h UngateCommand.h
GatePackage.o: /Include/TCLInterpreter.h /Include/TCLString.h
GatePackage.o: /usr/include/string.h /usr/include/features.h
GatePackage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GatePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
GatePackage.o: /usr/include/stdio.h
GatePackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
GatePackage.o: /usr/include/bits/types.h /usr/include/libio.h
GatePackage.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
Globals.o: /Include/TCLInterpreter.h /Include/TCLString.h
Globals.o: /usr/include/string.h /usr/include/features.h
Globals.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Globals.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Globals.o: /Include/histotypes.h /usr/include/math.h
Globals.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Globals.o: /usr/include/bits/mathcalls.h
Globals.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
MySpecTclApp.o: MySpecTclApp.h TclGrammerApp.h /Include/TCLApplication.h
MySpecTclApp.o: /Include/TCLVariable.h EventProcessor.h /Include/histotypes.h
MySpecTclApp.o: /usr/include/string.h /usr/include/features.h
MySpecTclApp.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
MySpecTclApp.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
MySpecTclApp.o: /usr/include/math.h /usr/include/bits/huge_val.h
MySpecTclApp.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
MySpecTclApp.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
MySpecTclApp.o: TCLAnalyzer.h /Include/TCLInterpreter.h /Include/TCLString.h
ParameterCommand.o: ParameterCommand.h /Include/TCLPackagedCommand.h
ParameterCommand.o: /Include/TCLCommandPackage.h /Include/TCLInterpreter.h
ParameterCommand.o: /Include/TCLString.h /usr/include/string.h
ParameterCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
ParameterCommand.o: /usr/include/gnu/stubs.h
ParameterCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ParameterCommand.o: ParameterPackage.h /Include/TCLResult.h
ParameterCommand.o: /Include/TCLList.h /usr/local/include/tcl.h
ParameterCommand.o: /usr/include/stdio.h
ParameterCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
ParameterCommand.o: /usr/include/bits/types.h /usr/include/libio.h
ParameterCommand.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
ParameterCommand.o: /usr/local/include/tclDecls.h /usr/include/malloc.h
ParameterPackage.o: ParameterPackage.h TCLHistogrammer.h ./PseudoScript.h
ParameterPackage.o: /Include/TCLInterpreterObject.h /usr/local/include/tcl.h
ParameterPackage.o: /usr/include/stdio.h /usr/include/features.h
ParameterPackage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ParameterPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ParameterPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
ParameterPackage.o: /usr/include/bits/types.h /usr/include/libio.h
ParameterPackage.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
ParameterPackage.o: /usr/local/include/tclDecls.h /Include/histotypes.h
ParameterPackage.o: /usr/include/string.h /usr/include/math.h
ParameterPackage.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
ParameterPackage.o: /usr/include/bits/mathcalls.h
ParameterPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
ParameterPackage.o: /usr/include/assert.h /Include/TCLObject.h
ParameterPackage.o: /Include/TCLList.h ParameterCommand.h
ParameterPackage.o: /Include/TCLPackagedCommand.h
ParameterPackage.o: /Include/TCLCommandPackage.h /Include/TCLInterpreter.h
ParameterPackage.o: /Include/TCLString.h PseudoCommand.h
PseudoCommand.o: PseudoCommand.h ParameterPackage.h /Include/TCLList.h
PseudoCommand.o: /Include/histotypes.h /usr/include/string.h
PseudoCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
PseudoCommand.o: /usr/include/gnu/stubs.h
PseudoCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
PseudoCommand.o: /usr/include/math.h /usr/include/bits/huge_val.h
PseudoCommand.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
PseudoCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
PseudoScript.o: ./PseudoScript.h /Include/TCLInterpreterObject.h
PseudoScript.o: /usr/local/include/tcl.h /usr/include/stdio.h
PseudoScript.o: /usr/include/features.h /usr/include/sys/cdefs.h
PseudoScript.o: /usr/include/gnu/stubs.h
PseudoScript.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
PseudoScript.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
PseudoScript.o: /usr/include/bits/types.h /usr/include/libio.h
PseudoScript.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
PseudoScript.o: /usr/local/include/tclDecls.h /Include/histotypes.h
PseudoScript.o: /usr/include/string.h /usr/include/math.h
PseudoScript.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
PseudoScript.o: /usr/include/bits/mathcalls.h
PseudoScript.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
PseudoScript.o: /usr/include/assert.h /Include/TCLObject.h /Include/TCLList.h
PseudoScript.o: /Include/TCLInterpreter.h /Include/TCLString.h
PseudoScript.o: /Include/TCLException.h /Include/TCLResult.h
PseudoScript.o: /Include/CException.h
ReadCommand.o: ReadCommand.h /Include/TCLPackagedCommand.h
ReadCommand.o: /usr/include/sys/stat.h /usr/include/features.h
ReadCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ReadCommand.o: /usr/include/bits/types.h
ReadCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ReadCommand.o: /usr/include/bits/stat.h /usr/include/sys/types.h
ReadCommand.o: /usr/include/time.h /usr/include/endian.h
ReadCommand.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ReadCommand.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ReadCommand.o: /usr/include/sys/sysmacros.h WriteCommand.h
ReadCommand.o: /Include/histotypes.h /usr/include/string.h
ReadCommand.o: /usr/include/math.h /usr/include/bits/huge_val.h
ReadCommand.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
ReadCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
ReadCommand.o: SpectrumPackage.h /Include/TCLInterpreter.h
ReadCommand.o: /Include/TCLString.h /Include/TCLResult.h /usr/include/fcntl.h
ReadCommand.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
ReadCommand.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
ReadCommand.o: /usr/include/getopt.h /usr/include/errno.h
ReadCommand.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
ReadCommand.o: /usr/include/asm/errno.h
RunControlPackage.o: RunControlPackage.h /Include/TCLCommandPackage.h
RunControlPackage.o: /Include/TCLInterpreter.h /Include/TCLString.h
RunControlPackage.o: /usr/include/string.h /usr/include/features.h
RunControlPackage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
RunControlPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
RunControlPackage.o: StartRun.h /Include/TCLPackagedCommand.h StopRun.h
RunControlPackage.o: Globals.h /Include/TCLVariable.h
SpectrumCommand.o: SpectrumCommand.h /Include/TCLPackagedCommand.h
SpectrumCommand.o: /Include/histotypes.h /usr/include/string.h
SpectrumCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
SpectrumCommand.o: /usr/include/gnu/stubs.h
SpectrumCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
SpectrumCommand.o: /usr/include/math.h /usr/include/bits/huge_val.h
SpectrumCommand.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
SpectrumCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
SpectrumCommand.o: SpectrumPackage.h /Include/TCLInterpreter.h
SpectrumCommand.o: /Include/TCLString.h /Include/TCLResult.h
SpectrumCommand.o: /Include/TCLCommandPackage.h /Include/TCLList.h
SpectrumCommand.o: /usr/local/include/tcl.h /usr/include/stdio.h
SpectrumCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
SpectrumCommand.o: /usr/include/bits/types.h /usr/include/libio.h
SpectrumCommand.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
SpectrumCommand.o: /usr/local/include/tclDecls.h /usr/include/malloc.h
SpectrumPackage.o: SpectrumPackage.h ClearCommand.h
SpectrumPackage.o: /Include/TCLPackagedCommand.h BindCommand.h
SpectrumPackage.o: UnbindCommand.h SpectrumCommand.h /Include/histotypes.h
SpectrumPackage.o: /usr/include/string.h /usr/include/features.h
SpectrumPackage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
SpectrumPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
SpectrumPackage.o: /usr/include/math.h /usr/include/bits/huge_val.h
SpectrumPackage.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
SpectrumPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
SpectrumPackage.o: ChanCommand.h Globals.h /Include/TCLInterpreter.h
SpectrumPackage.o: /Include/TCLString.h WriteCommand.h ReadCommand.h
SpectrumPackage.o: /usr/include/sys/stat.h /usr/include/bits/types.h
SpectrumPackage.o: /usr/include/bits/stat.h /usr/include/sys/types.h
SpectrumPackage.o: /usr/include/time.h /usr/include/endian.h
SpectrumPackage.o: /usr/include/bits/endian.h /usr/include/sys/select.h
SpectrumPackage.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
SpectrumPackage.o: /usr/include/sys/sysmacros.h /Include/TCLResult.h
SpectrumPackage.o: /usr/local/include/tcl.h /usr/include/stdio.h
SpectrumPackage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
SpectrumPackage.o: /usr/include/libio.h /usr/include/_G_config.h
SpectrumPackage.o: /usr/include/bits/stdio_lim.h
SpectrumPackage.o: /usr/local/include/tclDecls.h /usr/include/assert.h
SpectrumPackage.o: /usr/include/errno.h /usr/include/bits/errno.h
SpectrumPackage.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
SpectrumPackage.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
SpectrumPackage.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
SpectrumPackage.o: /usr/include/bits/posix2_lim.h
StartRun.o: StartRun.h /Include/TCLPackagedCommand.h
StartRun.o: /Include/TCLInterpreter.h /Include/TCLString.h
StartRun.o: /usr/include/string.h /usr/include/features.h
StartRun.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
StartRun.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
StartRun.o: /Include/TCLResult.h RunControlPackage.h
StartRun.o: /Include/TCLCommandPackage.h
StopRun.o: StopRun.h /Include/TCLInterpreter.h /Include/TCLString.h
StopRun.o: /usr/include/string.h /usr/include/features.h
StopRun.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
StopRun.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
StopRun.o: /Include/TCLResult.h RunControlPackage.h
StopRun.o: /Include/TCLCommandPackage.h
TCLAnalyzer.o: TCLAnalyzer.h /Include/TCLInterpreter.h /Include/TCLString.h
TCLAnalyzer.o: /usr/include/string.h /usr/include/features.h
TCLAnalyzer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLAnalyzer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
TCLAnalyzer.o: EventProcessor.h /Include/histotypes.h /usr/include/math.h
TCLAnalyzer.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
TCLAnalyzer.o: /usr/include/bits/mathcalls.h
TCLAnalyzer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
TCLHistogrammer.o: TCLHistogrammer.h ./PseudoScript.h
TCLHistogrammer.o: /Include/TCLInterpreterObject.h /usr/local/include/tcl.h
TCLHistogrammer.o: /usr/include/stdio.h /usr/include/features.h
TCLHistogrammer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TCLHistogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
TCLHistogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
TCLHistogrammer.o: /usr/include/bits/types.h /usr/include/libio.h
TCLHistogrammer.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TCLHistogrammer.o: /usr/local/include/tclDecls.h /Include/histotypes.h
TCLHistogrammer.o: /usr/include/string.h /usr/include/math.h
TCLHistogrammer.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
TCLHistogrammer.o: /usr/include/bits/mathcalls.h
TCLHistogrammer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
TCLHistogrammer.o: /usr/include/assert.h /Include/TCLObject.h
TCLHistogrammer.o: /Include/TCLList.h
TapeCommand.o: TapeCommand.h /Include/TCLInterpreter.h /Include/TCLString.h
TapeCommand.o: /usr/include/string.h /usr/include/features.h
TapeCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TapeCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
TapeCommand.o: /Include/TCLCommandPackage.h /Include/TCLResult.h
TapeCommand.o: DataSourcePackage.h /Include/histotypes.h /usr/include/math.h
TapeCommand.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
TapeCommand.o: /usr/include/bits/mathcalls.h
TapeCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
TclGrammerApp.o: TclGrammerApp.h /Include/TCLApplication.h
TclGrammerApp.o: /Include/TCLVariable.h /usr/include/limits.h
TclGrammerApp.o: /usr/include/features.h /usr/include/sys/cdefs.h
TclGrammerApp.o: /usr/include/gnu/stubs.h /usr/include/bits/posix1_lim.h
TclGrammerApp.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
TclGrammerApp.o: /usr/include/bits/posix2_lim.h /usr/include/assert.h
TclGrammerApp.o: TCLHistogrammer.h ./PseudoScript.h
TclGrammerApp.o: /Include/TCLInterpreterObject.h /usr/local/include/tcl.h
TclGrammerApp.o: /usr/include/stdio.h
TclGrammerApp.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
TclGrammerApp.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
TclGrammerApp.o: /usr/include/bits/types.h /usr/include/libio.h
TclGrammerApp.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
TclGrammerApp.o: /usr/local/include/tclDecls.h /Include/histotypes.h
TclGrammerApp.o: /usr/include/string.h /usr/include/math.h
TclGrammerApp.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
TclGrammerApp.o: /usr/include/bits/mathcalls.h
TclGrammerApp.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
TclGrammerApp.o: /Include/TCLObject.h /Include/TCLList.h
TclGrammerApp.o: /Include/TCLProcessor.h /Include/TCLInterpreter.h
TclGrammerApp.o: /Include/TCLString.h /Include/TCLResult.h Globals.h
TclGrammerApp.o: RunControlPackage.h /Include/TCLCommandPackage.h
TclGrammerApp.o: ParameterPackage.h SpectrumPackage.h DataSourcePackage.h
TclGrammerApp.o: GatePackage.h GateCommand.h /Include/TCLPackagedCommand.h
TclGrammerApp.o: GateFactory.h ApplyCommand.h UngateCommand.h TCLAnalyzer.h
TclGrammerApp.o: XamineEventHandler.h /Include/TCLFileHandler.h
TclGrammerApp.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
TclGrammerApp.o: /usr/include/bits/confname.h /usr/include/getopt.h
TclGrammerApp.o: /usr/local/include/tk.h /usr/include/X11/Xlib.h
TclGrammerApp.o: /usr/include/sys/types.h /usr/include/time.h
TclGrammerApp.o: /usr/include/endian.h /usr/include/bits/endian.h
TclGrammerApp.o: /usr/include/sys/select.h /usr/include/bits/select.h
TclGrammerApp.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
TclGrammerApp.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
TclGrammerApp.o: /usr/include/X11/Xosdefs.h /usr/local/include/tkDecls.h
TclGrammerApp.o: /usr/include/stdlib.h /usr/include/alloca.h
TclGrammerApp.o: /usr/include/sys/sysinfo.h /usr/include/linux/kernel.h
UnbindCommand.o: UnbindCommand.h /Include/TCLInterpreter.h
UnbindCommand.o: /Include/TCLString.h /usr/include/string.h
UnbindCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
UnbindCommand.o: /usr/include/gnu/stubs.h
UnbindCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
UnbindCommand.o: /Include/TCLCommandPackage.h /Include/TCLResult.h
UnbindCommand.o: SpectrumPackage.h /Include/histotypes.h /usr/include/math.h
UnbindCommand.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
UnbindCommand.o: /usr/include/bits/mathcalls.h
UnbindCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
UngateCommand.o: UngateCommand.h GatePackage.h GateCommand.h
UngateCommand.o: /Include/TCLPackagedCommand.h GateFactory.h ApplyCommand.h
UngateCommand.o: /Include/TCLInterpreter.h /Include/TCLString.h
UngateCommand.o: /usr/include/string.h /usr/include/features.h
UngateCommand.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
UngateCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
UngateCommand.o: /Include/TCLResult.h /usr/include/assert.h
UserCode.o: /Include/histotypes.h /usr/include/string.h
UserCode.o: /usr/include/features.h /usr/include/sys/cdefs.h
UserCode.o: /usr/include/gnu/stubs.h
UserCode.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
UserCode.o: /usr/include/math.h /usr/include/bits/huge_val.h
UserCode.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
UserCode.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
WriteCommand.o: WriteCommand.h SpectrumPackage.h /usr/include/errno.h
WriteCommand.o: /usr/include/features.h /usr/include/sys/cdefs.h
WriteCommand.o: /usr/include/gnu/stubs.h /usr/include/bits/errno.h
WriteCommand.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
WriteCommand.o: /usr/include/string.h
WriteCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
WriteCommand.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
WriteCommand.o: /usr/include/sys/types.h /usr/include/bits/types.h
WriteCommand.o: /usr/include/time.h /usr/include/endian.h
WriteCommand.o: /usr/include/bits/endian.h /usr/include/sys/select.h
WriteCommand.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
WriteCommand.o: /usr/include/sys/sysmacros.h /usr/include/sys/stat.h
WriteCommand.o: /usr/include/bits/stat.h /usr/include/stdio.h
WriteCommand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
WriteCommand.o: /usr/include/libio.h /usr/include/_G_config.h
WriteCommand.o: /usr/include/bits/stdio_lim.h /usr/include/unistd.h
WriteCommand.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
WriteCommand.o: /usr/include/getopt.h
XamineEventHandler.o: XamineEventHandler.h /Include/TCLFileHandler.h
XamineEventHandler.o: /usr/include/unistd.h /usr/include/features.h
XamineEventHandler.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XamineEventHandler.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
XamineEventHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XamineEventHandler.o: /usr/include/bits/confname.h /usr/include/getopt.h
XamineEventHandler.o: /usr/include/stdio.h
XamineEventHandler.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
XamineEventHandler.o: /usr/include/libio.h /usr/include/_G_config.h
XamineEventHandler.o: /usr/include/bits/stdio_lim.h /usr/local/include/tk.h
XamineEventHandler.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
XamineEventHandler.o: /usr/include/time.h /usr/include/endian.h
XamineEventHandler.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XamineEventHandler.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XamineEventHandler.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h
XamineEventHandler.o: /usr/include/X11/Xfuncproto.h
XamineEventHandler.o: /usr/include/X11/Xosdefs.h /usr/local/include/tkDecls.h
XamineEventHandler.o: GateFactory.h GatePackage.h GateCommand.h
XamineEventHandler.o: /Include/TCLPackagedCommand.h ApplyCommand.h
XamineEventHandler.o: UngateCommand.h /usr/include/assert.h
