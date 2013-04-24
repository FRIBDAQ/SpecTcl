DEFS = @WCHAR_T_DEF@
INCLUDES = -I@top_srcdir@ -I. -I@top_srcdir@/TCL -I@top_srcdir@/Events \
	-I@top_srcdir@/Utility -I@top_srcdir@/Sorter  \
	-I@top_srcdir@/NSCLException \
	-I@top_srcdir@/EventSource -I@top_srcdir@/Gates -I@top_srcdir@/SpectrumIO \
	-I@top_srcdir@/Xamine \
	-I@top_srcdir@/Display -I@top_srcdir@/Tape -I@top_srcdir@/Filter \
	-I@top_srcdir@/Analysis -I@top_srcdir@/TreeParam \
	-I@top_srcdir@/Fits                               \
	-I@top_srcdir@/factories			\
	-I@top_srcdir@/DecoderRing			\
	@TCL_FLAGS@ -DINSTALLED_IN='"$(prefix)"' @XINCLUDES@

install-exec-local:
	$(mkinstalldirs) $(prefix)/etc
	$(mkinstalldirs) $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclInit.tcl $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.cpp $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.h $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclRC.tcl $(prefix)/Skel
	$(INSTALL_DATA)    $(srcdir)/Globals.o $(prefix)/lib
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
	ScalerProcessor.cpp ScalerProcessorCallbacks.cpp \
	DocumentationException.cpp DocumentationCallback.cpp \
	SpecTcl.cpp CFoldCommand.cpp CProjectionCommand.cpp CFitCommand.cpp \
	IntegrateCommand.cpp RingFormatCommand.cpp


include_HEADERS = AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h \
	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h \
	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h \
	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h \
	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h \
	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h \
	TCLAnalyzer.h TCLHistogrammer.h TapeCommand.h TclGrammerApp.h \
	UnbindCommand.h UngateCommand.h WriteCommand.h XamineEventHandler.h \
	FilterCommand.h \
	ScalerProcessor.h ScalerProcessorCallbacks.h \
	DocumentationException.h DocumentationCallback.h \
	SpecTcl.h CFoldCommand.h CProjectionCommand.h CFitCommand.h \
	IntegrateCommand.h

        




bin_PROGRAMS = SpecTcl

SpecTcl_DEPENDENCIES =  libTclGrammerCommands.la Globals.o

SpecTcl_SOURCES = MySpecTclApp.cpp \
	AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h \
	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h \
	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h \
	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h \
	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h \
	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h \
	TCLAnalyzer.h TCLHistogrammer.h TapeCommand.h TclGrammerApp.h \
	UnbindCommand.h UngateCommand.h WriteCommand.h XamineEventHandler.h  \
	CFoldCommand.h



AM_CXXFLAGS = -g

SpecTcl_LDFLAGS = -lTclGrammerCommands -lXplus -lGates -lAnalysis \
	-lFilter -lBufferAnalysis -lEventSource -lSpecio -ltclPlus \
	-lException -lTreeParam -lXamine  -ltape -lFits \
	@top_srcdir@/Sorter/libSorting.la		\
	@top_srcdir@/DecoderRing/libDecoderRing.la	\
	-lXt -lX11 $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS)  \
	-L. -L../SpectrumIO \
	-L../Tape -L../Sorter \
	-L../Xamine -L../Display -L../Gates -L../EventSource -L../Events \
	-L../TCL -L../NSCLException -L../Filter -L../Analysis -L../TreeParam \
	-L../Fits -L@top_srcdir@/DecoderRing	\
	-g -Wl,"-rpath=$(libdir)" -lgsl -lgslcblas  @TCL_LDFLAGS@ @XLIBSW@


man_MANS = spectclapi.3


EXTRA_DIST = Makefile.skel SPECTCL.WC SpecTclInit.tcl SpecTclRC.tcl \
	 mapped.tcl paramtest.tcl  \
	SpecTcl_Makefile.include $(man_MANS)




# Unit tests.

if CPPUNIT


noinst_PROGRAMS =  scalertests docbuftests


# TODO: Could probably add quite a few more headers above to this.
#       maybe *Command.h and *Package.h(?)


noinst_HEADERS = Asserts.h RingFormatCommand.h

scalertests_SOURCES = TestRunner.cpp \
	scalercallbacktests.cpp 	\
	controlcallbacktests.cpp

scalertests_DEPENDENCIES = ScalerProcessorCallbacks.o

docbuftests_SOURCES = TestRunner.cpp \
	docexceptiontests.cpp \
	callbacktests.cpp 

docbuftests_DEPENDENCIES = DocumentationException.o  \
	DocumentationCallback.o


commonTestLdFlags= -L@top_srcdir@/Events -lAnalysis \
	-L@top_srcdir@/TCL -ltclPlus \
	-L@top_srcdir@/NSCLException -lException \
	-L@top_srcdir@/Analysis -lBufferAnalysis \
	$(CPPUNIT_LDFLAGS) $(TCL_LDFLAGS)

scalertests_LDFLAGS= ScalerProcessorCallbacks.o $(commonTestLdFlags)

docbuftests_LDFLAGS = DocumentationException.o DocumentationCallback.o \
	$(commonTestLdFlags)

TESTS = ./scalertests ./docbuftests

endif
