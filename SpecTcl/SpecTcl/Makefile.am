DEFS = @WCHAR_T_DEF@
INCLUDES = -I.. -I. -I../TCL -I../Events -I../Utility -I../Sorter  \
	-I../NSCLException \
	-I../EventSource -I../Gates -I../SpectrumIO -I../Xamine \
	-I../Display -I../Tape -I../Filter -I../Analysis \
	@TCL_FLAGS@ -DINSTALLED_IN='"$(prefix)"' @XINCLUDES@

install-exec-local:
	$(mkinstalldirs) $(prefix)/etc
	$(mkinstalldirs) $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclInit.tcl $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.cpp $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.h $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclRC.tcl $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/setup.tcl $(prefix)/Skel
	$(INSTALL_DATA)    $(srcdir)/Globals.o $(prefix)/lib
	#
	#    Build Makefile for the user.	
	#
	$(INSTALL_DATA)   SpecTcl_Makefile.include $(sysconfdir)
	echo INSTDIR=$(prefix) >Makefile.user
	cat Makefile.skel >> Makefile.user
	$(INSTALL_PROGRAM) $(srcdir)/Makefile.user $(prefix)/Skel/Makefile


lib_LTLIBRARIES = libTclGrammerCommands.la

libTclGrammerCommands_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libTclGrammerCommands_la_SOURCES = StopRun.cpp StartRun.cpp \
	RunControlPackage.cpp ParameterPackage.cpp ParameterCommand.cpp \
	SpectrumPackage.cpp ClearCommand.cpp BindCommand.cpp \
	UnbindCommand.cpp SpectrumCommand.cpp DataSourcePackage.cpp \
	AttachCommand.cpp TapeCommand.cpp GateCommand.cpp ApplyCommand.cpp \
	UngateCommand.cpp GatePackage.cpp GateFactory.cpp \
	GateFactoryException.cpp XamineEventHandler.cpp TCLHistogrammer.cpp \
	PseudoCommand.cpp PseudoScript.cpp ChanCommand.cpp WriteCommand.cpp \
	ReadCommand.cpp TCLAnalyzer.cpp EventProcessor.cpp TclGrammerApp.cpp \
	Globals.cpp FilterCommand.cpp \
	ScalerProcessor.cpp ScalerProcessorCallbacks.cpp

include_HEADERS = AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h \
	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h \
	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h \
	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h \
	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h \
	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h \
	TCLAnalyzer.h TCLHistogrammer.h TapeCommand.h TclGrammerApp.h \
	UnbindCommand.h UngateCommand.h WriteCommand.h XamineEventHandler.h \
	FilterCommand.h \
	ScalerProcessor.h ScalerProcessorCallbacks.h

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


SpecTcl_LDFLAGS = -g -Wl,"-rpath=$(libdir)"

AM_CXXFLAGS = -g

SpecTcl_LDADD = -lTclGrammerCommands -lSorting -lXplus -lGates -lAnalysis \
	-lFilter -lBufferAnalysis -lEventSource -lSpecio -ltclPlus -lException -lXamine  -ltape -lXt -lX11 $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS)
SpecTcl_LDFLAGS = -L. @TCL_LDFLAGS@ @XLIBSW@ -L../SpectrumIO -L../Tape -L../Sorter -L../Xamine -L../Display -L../Gates -L../EventSource -L../Events -L../TCL -L../NSCLException -L../Filter -L../Analysis

EXTRA_DIST = Makefile.skel SPECTCL.WC SpecTclInit.tcl SpecTclRC.tcl \
		 mapped.tcl paramtest.tcl setup.tcl \
	SpecTcl_Makefile.include


# Unit tests.

if CPPUNIT


noinst_PROGRAMS =  scalertests

noinst_HEADERS = Asserts.h

scalertests_SOURCES = TestRunner.cpp \
	scalercallbacktests.cpp 	\
	controlcallbacktests.cpp \
	ScalerProcessorCallbacks.cpp 

scalertests_LDFLAGS= -L@top_srcdir@/Events -lAnalysis \
	-L@top_srcdir@/TCL -ltclPlus \
	-L@top_srcdir@/NSCLException -lException \
	-L@top_srcdir@/Analysis -lBufferAnalysis \
	$(CPPUNIT_LDFLAGS)

TESTS = ./scalertests

endif
