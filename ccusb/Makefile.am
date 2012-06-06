bin_PROGRAMS=CCUSBSpecTcl

CCUSBSpecTcl_SOURCES=CC1205Unpacker.cpp \
	CCCUSBPacket.cpp \
	CCUSBUnpacker.cpp \
	CFixedSizedUnpacker.cpp \
	CPh7xxUnpacker.cpp \
	MySpecTclApp.cpp \
	ParamMapCommand.cpp 

noinst_HEADERS=CC1205Unpacker.h \
	CCCUSBPacket.h \
	CCUSBUnpacker.h \
	CFixedSizedUnpacker.h \
	CPh7xxUnpacker.h \
	MySpecTclApp.h \
	ParamMapCommand.h


INCLUDES =  -I. -I@top_srcdir@  -I@top_srcdir@/TCL -I@top_srcdir@/Events \
	-I@top_srcdir@/Utility -I@top_srcdir@/Sorter  \
	-I@top_srcdir@/NSCLException \
	-I@top_srcdir@/EventSource -I@top_srcdir@/Gates \
	-I@top_srcdir@/SpectrumIO -I@top_srcdir@/Xamine -I@top_srcdir@/SpecTcl \
	-I@top_srcdir@/Analysis \
	-I@top_srcdir@/Display -I@top_srcdir@/Tape -I@top_srcdir@/Filter \
	-I@top_srcdir@/TreeParam \
	@TCL_FLAGS@ -DINSTALLED_IN='"$(prefix)"' @XINCLUDES@

CCUSBSpecTcl_LDFLAGS = ../Events/libAnalysis.la \
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


#
#  This stuff installs the code in the CCUSBSkel directory so that
#  people can use it as a starting point.  For now, only the
#  SpecTclRC.tcl script is pushed there as we're going to put the
#  other scripts where they can be found in TclLibs.
#
install-exec-local:	docs_built
	$(mkinstalldirs) @prefix@/CCUSBSkel
	$(INSTALL_DATA) *.cpp @prefix@/CCUSBSkel
	$(INSTALL_DATA) *.h   @prefix@/CCUSBSkel
	$(INSTALL_DATA) SpecTclRC.tcl @prefix@/CCUSBSkel
	$(INSTALL_DATA) MakefileSkel @prefix@/CCUSBSkel/Makefile
	$(mkinstalldirs) @prefix@/TclLibs/ccusb
	$(INSTALL_DATA)  spectclconfig.tcl @prefix@/TclLibs/ccusb
	echo pkg_mkIndex @prefix@/TclLibs/ccusb *.tcl | $(TCLSH_CMD)
	$(mkinstalldirs) @prefix@/share/ccusb
	$(INSTALL_DATA)  *.html @prefix@/share/ccusb
	$(INSTALL_DATA)  *.pdf  @prefix@/share/ccusb
	-for s in {1..8} ; do		\
		$(mkinstalldirs) @prefix@/share/man/man$$s; \
		for f in *.$$s*; do  \
			$(INSTALL_DATA) $$f @prefix@/share/man/man$$s; \
		done \
	done


EXTRA_DIST=MakefileSkel.in \
	spectclconfig.tcl  \
	SpecTclRC.tcl ccusbSpecTcl.xml

docs_built: ccusbSpecTcl.xml
	-$(MANDOCBOOK) man $< 2>/dev/null
	$(HTMLDOCBOOK) $<
	$(HCDOCBOOK) $<
	dvipdf       ccusbSpecTcl.dvi
	touch docs_built


