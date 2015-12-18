bin_PROGRAMS=VMUSBSpecTcl

VMUSBSpecTcl_SOURCES = C785Unpacker.cpp \
CFitButton.cpp \
CHINP.cpp \
CMADC32Unpacker.cpp \
CMASE.cpp \
CModuleUnpacker.cpp \
CNADC2530Unpacker.cpp \
CParamMapCommand.cpp \
CPSD.cpp \
CRateCommand.cpp \
CRateEventProcessor.cpp \
CRateEventProcssor.cpp \
CRateList.cpp \
CRateProcessor.cpp \
CStackMapCommand.cpp \
CStackUnpacker.cpp \
CV1729Unpacker.cpp \
CV1x90Unpacker.cpp \
CV977Unpacker.cpp \
LICENCE.cpp \
MySpecTclApp.cpp 


noinst_HEADERS = C785Unpacker.h \
CFitButton.h \
CHINP.h \
CLLNLUnpacker.h \
CMADC32Unpacker.h \
CMASE.h \
CModuleUnpacker.h \
CNADC2530Unpacker.h \
CParamMapCommand.h \
CPSD.h \
CRateCommand.h \
CRateEventProcessor.h \
CRateEventProcssor.h \
CRateList.h \
CRateProcessor.h \
CStackMapCommand.h \
CStackUnpacker.h \
CV1729Unpacker.h \
CV1x90Unpacker.h \
CV977Unpacker.h \
MySpecTclApp.h


AM_CPPFLAGS =  -I. -I@top_srcdir@  \
	-I@top_srcdir@/Events \
	-I@top_srcdir@/Utility \
	-I@top_srcdir@/Sorter  \
	-I@top_srcdir@/EventSource \
	-I@top_srcdir@/Gates \
	-I@top_srcdir@/SpectrumIO \
	-I@top_srcdir@/Xamine \
	-I@top_srcdir@/SpecTcl \
	-I@top_srcdir@/Analysis \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/DisplayInterface \
	-I@top_srcdir@/Tape \
	-I@top_srcdir@/Filter \
	-I@top_srcdir@/TreeParam \
	-I@top_srcdir@/Fits \
	-I@top_srcdir@/factories    \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@

VMUSBSpecTcl_LDADD = @top_builddir@/SpecTcl/libTclGrammerCommands.la \
		    @top_builddir@/Events/libAnalysis.la \
		    @top_builddir@/Fits/libFits.la \
		    @top_builddir@/Analysis/libBufferAnalysis.la \
		    @top_builddir@/EventSource/libEventSource.la \
		    @top_builddir@/Tape/libtape.la \
		    @top_builddir@/Gates/libGates.la \
		    @top_builddir@/Replace/libreplace.la \
		    @top_builddir@/Xamine/libXplus.la \
		    @top_builddir@/calibratedparams/libCalibrations.la \
		    @top_builddir@/Sorter/libSorting.la \
		    @top_builddir@/Filter/libFilter.la \
		    @top_builddir@/SpectrumIO/libSpecio.la \
		    @top_builddir@/TreeParam/libTreeParam.la \
		    @top_builddir@/DecoderRing/libDecoderRing.la \
		    @top_builddir@/Display/libXamine.la          \
		    @top_builddir@/DisplayInterface/libDisplayInterface.la

VMUSBSpecTcl_LDFLAGS = 	\
	@LIBTCLPLUS_LDFLAGS@	\
	-lXt -lX11 @X11EXTRA_LDFLAGS@  @LD_EXTRA_FLAGS@ \
	-lgsl -lgslcblas	\
	 @TK_LIBS@ @TCL_LIBS@ @XLIBSW@ -L@top_srcdir@/SpectrumIO \
	 -g -Wl,"-rpath=@libdir@"


install-exec-local: docs_built
	$(mkinstalldirs) @prefix@/VMUSBSkel @prefix@/share/vmusb
	$(INSTALL_DATA) *.cpp *.h        @prefix@/VMUSBSkel
	$(INSTALL_DATA) MakefileSkel     @prefix@/VMUSBSkel/Makefile
	$(mkinstalldirs) @prefix@/TclLibs/vmusb
	$(INSTALL_DATA) spectclSetup.tcl configFile.tcl constants.tcl  @prefix@/TclLibs/vmusb
	echo pkg_mkIndex -verbose @prefix@/TclLibs/vmusb "*.tcl" | @TCLSH@
	$(mkinstalldirs) @prefix@/share/vmusb
	$(INSTALL_DATA)  *.html @prefix@/share/vmusb
	$(INSTALL_DATA)  *.pdf  @prefix@/share/vmusb
	-for s in {1..8} ; do		\
		$(mkinstalldirs) @prefix@/share/man/man$$s; \
		for f in *.$$s*; do  \
			$(INSTALL_DATA) $$f @prefix@/share/man/man$$s; \
		done \
	done


EXTRA_DIST = MakefileSkel.in configFile.tcl spectclSetup.tcl CLLNLUnpacker.cpp  vmusb.xml constants.tcl



docs_built: vmusb.xml
	-@MANDOCBOOK@ man $< 2>/dev/null
	@HTMLDOCBOOK@ $<
	@HCDOCBOOK@ $<
	dvipdf       vmusb.dvi
	touch docs_built

