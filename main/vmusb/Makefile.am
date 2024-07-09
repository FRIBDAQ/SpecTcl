lib_LTLIBRARIES=libVMUSBSpecTcl.la

COMMON_cppflags = -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core  \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/factories    \
	-I@top_srcdir@/mpi \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@ @ROOT_CFLAGS@ -I@top_srcdir@/mpi

libVMUSBSpecTcl_la_SOURCES=CFitButton.cpp \
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
C785Unpacker.cpp \
CV1729Unpacker.cpp \
CV1x90Unpacker.cpp \
CV977Unpacker.cpp \
CMTDC32Unpacker.cpp \
CMQDC32Unpacker.cpp \
CMDPP32QDCUnpacker.cpp \
CMDPP32SCPSROUnpacker.cpp \
CVMUSBScalerUnpacker.cpp \
LICENCE.cpp

libVMUSBSpecTcl_la_CPPFLAGS=$(COMMON_cppflags)

include_HEADERS= C785Unpacker.h \
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
CV1729Unpacker.h \
CV1x90Unpacker.h \
CV977Unpacker.h \
CMTDC32Unpacker.h \
CMQDC32Unpacker.h \
CMDPP32QDCUnpacker.h \
CMDPP32SCPSROUnpacker.h \
CVMUSBScalerUnpacker.h

bin_PROGRAMS=VMUSBSpecTcl

VMUSBSpecTcl_SOURCES = CStackUnpacker.cpp \
MySpecTclApp.cpp 


noinst_HEADERS =CStackUnpacker.h \
MySpecTclApp.h

# Removed: 	-I@top_srcdir@/Tape 




VMUSBSpecTcl_CPPFLAGS = $(COMMON_cppflags)

# Removed 		    @top_builddir@/Tape/libtape.la 


VMUSBSpecTcl_DEPENDENCIES=libVMUSBSpecTcl.la

VMUSBSpecTcl_LDADD =@builddir@/libVMUSBSpecTcl.la @top_builddir@/Core/libSpecTclMain.la \
		@top_builddir@/Core/libTclGrammerApp.la \
		    @top_builddir@/calibratedparams/libCalibrations.la \
		    @top_builddir@/Display/libXamine.la \
			@top_builddir@/mpi/libMPIUtilities.la \
	-lXt -lX11 @X11EXTRA_LDFLAGS@  @LD_EXTRA_FLAGS@ @ROOT_LDFLAGS@ \
	-lgsl -lgslcblas	\
	 @TK_LIBS@ @TCL_LIBS@ @XLIBSW@


VMUSBSpecTcl_LDFLAGS = 	 -g -Wl,"-rpath=@libdir@"


install-exec-local:
	$(mkinstalldirs) @prefix@/VMUSBSkel @prefix@/share/vmusb
	for f in $(VMUSBSpecTcl_SOURCES) ; do $(INSTALL_DATA) @srcdir@/$$f  @prefix@/VMUSBSkel ; done
	for f in $(noinst_HEADERS); do $(INSTALL_DATA) @srcdir@/$$f  @prefix@/VMUSBSkel ; done
	$(INSTALL_DATA) MakefileSkel     @prefix@/VMUSBSkel/Makefile
	$(INSTALL_SCRIPT) @srcdir@/SpecTclRC.tcl @prefix@/VMUSBSkel
	$(INSTALL_SCRIPT) @srcdir@/SpecTclInit.tcl @prefix@/VMUSBSkel
	$(mkinstalldirs) @prefix@/TclLibs/vmusb
	$(INSTALL_DATA) @srcdir@/spectclSetup.tcl @srcdir@/configFile.tcl @srcdir@/constants.tcl  @prefix@/TclLibs/vmusb
	echo pkg_mkIndex -verbose @prefix@/TclLibs/vmusb "*.tcl" | @TCLSH@
	$(mkinstalldirs) @prefix@/share/vmusb
	for f in @builddir@/*.html ; do $(INSTALL_DATA) $$f @prefix@/share/vmusb ; done
	for f in @builddir@/*.pdf ; do $(INSTALL_DATA)  $$f @prefix@/share/vmusb ; done
	-for s in {1..8} ; do		\
		$(mkinstalldirs) @prefix@/share/man/man$$s; \
		for f in *.$$s*; do  \
			$(INSTALL_DATA) $$f @prefix@/share/man/man$$s; \
		done \
	done


EXTRA_DIST = MakefileSkel.in configFile.tcl spectclSetup.tcl CLLNLUnpacker.cpp  vmusb.xml constants.tcl SpecTclRC.tcl

BUILT_SOURCES = @builddir@/docs_built



@builddir@/docs_built: @srcdir@/vmusb.xml
	@MANDOCBOOK@ man $< -o @builddir@ 2>/dev/null
	@HTMLDOCBOOK@ $< -o @builddir@
	@HCDOCBOOK@ $< -o @builddir@
	dvipdf       @builddir@/vmusb.dvi @builddir@/vmusb.pdf
	touch @builddir@/docs_built


clean-local:
	rm -f @builddir@/docs_built

