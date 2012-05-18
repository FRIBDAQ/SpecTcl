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


INCLUDES =  -I. -I@top_srcdir@  -I@top_srcdir@/TCL -I@top_srcdir@/Events \
	-I@top_srcdir@/Utility -I@top_srcdir@/Sorter  \
	-I@top_srcdir@/NSCLException \
	-I@top_srcdir@/EventSource -I@top_srcdir@/Gates \
	-I@top_srcdir@/SpectrumIO -I@top_srcdir@/Xamine -I@top_srcdir@/SpecTcl \
	-I@top_srcdir@/Analysis \
	-I@top_srcdir@/Display -I@top_srcdir@/Tape -I@top_srcdir@/Filter \
	-I@top_srcdir@/TreeParam \
	-I@top_srcdir@/Fits  -I@top_srcdir@/factories    \
	@TCL_FLAGS@ -DINSTALLED_IN='"$(prefix)"' @XINCLUDES@

VMUSBSpecTcl_LDFLAGS =  ../Events/libAnalysis.la \
	../Fits/libFits.la \
	../TCL/libtclPlus.la \
	../Analysis/libBufferAnalysis.la \
	../EventSource/libEventSource.la \
	../Tape/libtape.la \
	../Gates/libGates.la \
	../Replace/libreplace.la \
	../Xamine/libXplus.la \
	../calibratedparams/libCalibrations.la \
	../Sorter/libSorting.la \
	../Filter/libFilter.la \
	../SpectrumIO/libSpecio.la \
	../TreeParam/libTreeParam.la \
	../SpecTcl/libTclGrammerCommands.la \
	../DecoderRing/libDecoderRing.la \
	../NSCLException/libException.la \
	../Display/libXamine.la          \
	-lXt -lX11 $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS) \
	-lgsl -lgslcblas	\
	 @TCL_LDFLAGS@ @XLIBSW@ -L@top_srcdir@/SpectrumIO \
	 -g -Wl,"-rpath=($prefix)/lib"


install-exec-local: docs_built
	$(mkinstalldirs) @prefix@/VMUSBSkel
	$(INSTALL_DATA) *.cpp *.h        @prefix@/VMUSBSkel
	$(INSTALL_DATA) MakefileSkel     @prefix@/VMUSBSkel/Makefile
	$(mkinstalldirs) @prefix@/TclLibs/vmusb
	$(INSTALL_DATA) spectclSetup.tcl configFile.tcl constants.tcl  @prefix@/TclLibs/vmusb
	echo pkg_mkIndex -verbose @prefix@/TclLibs/vmusb "*.tcl" | $(TCLSH_CMD)


EXTRA_DIST = MakefileSkel.in configFile.tcl spectclSetup.tcl CLLNLUnpacker.cpp 



docs_built: 