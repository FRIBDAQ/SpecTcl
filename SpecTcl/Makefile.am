DEFS = @WCHAR_T_DEF@
INCLUDES = -I.. -I. -I../TCL -I../Events -I../Utility -I../Sorter  \
	-I../NSCLException \
	-I../EventSource -I../Gates -I../SpectrumIO -I../Xamine \
	-I../Display -I../Tape -I../Filter \
	@TCL_FLAGS@ -DINSTALLED_IN='"$(prefix)"' @XINCLUDES@

install-exec-local:
	$(mkinstalldirs) $(prefix)/Etc
	$(mkinstalldirs) $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclInit.tcl $(prefix)/Etc
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.cpp $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.h $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclRC.tcl $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/setup.tcl $(prefix)/Skel
	$(INSTALL_DATA)    $(srcdir)/Globals.o $(prefix)/lib
	#    Build Makefile for the user.	
	#    first the environment.	
	#
	echo INSTDIR=$(prefix) >Makefile.user
	echo OS=$(OS)|  cut -d- -f1      >>Makefile.user
	echo >>                            Makefile.user
	#
	#  Now the typical flags we need:
	#
	echo INCLUDES="@TCL_FLAGS@ @XINCLUDES@ -I."  \
	                                        >>Makefile.user
	echo LIBDIRS="@TCL_LDFLAGS@ @XLIBSW@ " >> Makefile.user
	echo LIBFLAGS="@X_PRE_LIBS@"           >> Makefile.user
	echo                                   >> Makefile.user
	#
	#   Now the compiler commands:
	#
	echo CPP=$(CPP)                        >> Makefile.user
	echo CXX=$(CXX)                        >> Makefile.user
	echo CC=$(CC)                          >> Makefile.user
	echo LD=$(CXX)                         >> Makefile.user
	echo                                   >> Makefile.user
	#
	#   Now the rest of the makefile:
	#
	cat Makefile.skel >> Makefile.user
	$(INSTALL_PROGRAM) $(srcdir)/Makefile.user $(prefix)/Skel/Makefile


lib_LTLIBRARIES = libTclGrammerCommands.la

libTclGrammerCommands_la_LDFLAGS = -version-info $(SOVERSION):0

libTclGrammerCommands_la_SOURCES = StopRun.cpp StartRun.cpp \
	RunControlPackage.cpp ParameterPackage.cpp ParameterCommand.cpp \
	SpectrumPackage.cpp ClearCommand.cpp BindCommand.cpp \
	UnbindCommand.cpp SpectrumCommand.cpp DataSourcePackage.cpp \
	AttachCommand.cpp TapeCommand.cpp GateCommand.cpp ApplyCommand.cpp \
	UngateCommand.cpp GatePackage.cpp GateFactory.cpp \
	GateFactoryException.cpp XamineEventHandler.cpp TCLHistogrammer.cpp \
	PseudoCommand.cpp PseudoScript.cpp ChanCommand.cpp WriteCommand.cpp \
	ReadCommand.cpp TCLAnalyzer.cpp EventProcessor.cpp TclGrammerApp.cpp \
	Globals.cpp FilterCommand.cpp

include_HEADERS = AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h \
	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h \
	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h \
	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h \
	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h \
	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h \
	TCLAnalyzer.h TCLHistogrammer.h TapeCommand.h TclGrammerApp.h \
	UnbindCommand.h UngateCommand.h WriteCommand.h XamineEventHandler.h \
	FilterCommand.h

bin_PROGRAMS = SpecTcl

SpecTcl_DEPENDENCIES =  libTclGrammerCommands.la

SpecTcl_SOURCES = MySpecTclApp.cpp \
	AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h \
	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h \
	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h \
	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h \
	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h \
	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h \
	TCLAnalyzer.h TCLHistogrammer.h TapeCommand.h TclGrammerApp.h \
	UnbindCommand.h UngateCommand.h WriteCommand.h XamineEventHandler.h


SpecTcl_LDFLAGS = -g

AM_CXXFLAGS = -g

SpecTcl_LDADD = -lTclGrammerCommands -lSorting -lXplus -lGates -lAnalysis \
	-lFilter -lEventSource -lSpecio -ltclPlus -lException -lXamine  -ltape -lXt -lX11 $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS)
SpecTcl_LDFLAGS = -L. @TCL_LDFLAGS@ @XLIBSW@ -L../SpectrumIO -L../Tape -L../Sorter -L../Xamine -L../Display -L../Gates -L../EventSource -L../Events -L../TCL -L../NSCLException -L../Filter

EXTRA_DIST = Makefile.skel SPECTCL.WC SpecTclInit.tcl SpecTclRC.tcl classbreakdown.WC classbreakdown.omt mapped.tcl paramtest.tcl setup.tcl
