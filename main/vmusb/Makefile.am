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


VMUSBSpecTcl_CPPFLAGS = -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core  \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/Tape \
	-I@top_srcdir@/factories    \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@

VMUSBSpecTcl_LDADD =@top_builddir@/Core/libTclGrammerApp.la \
		    @top_builddir@/Tape/libtape.la \
		    @top_builddir@/calibratedparams/libCalibrations.la \
		    @top_builddir@/Display/libXamine.la

VMUSBSpecTcl_LDFLAGS = 	\
	@LIBTCLPLUS_LDFLAGS@	\
	-lXt -lX11 @X11EXTRA_LDFLAGS@  @LD_EXTRA_FLAGS@ \
	-lgsl -lgslcblas	\
	 @TK_LIBS@ @TCL_LIBS@ @XLIBSW@ \
	 -g -Wl,"-rpath=@libdir@"


install-exec-local: docs_built
	$(mkinstalldirs) @prefix@/VMUSBSkel @prefix@/share/vmusb
	for f in @srcdir@/*.cpp ; do $(INSTALL_DATA) $$f @prefix@/VMUSBSkel ; done
	for f in @srcdir@/*.h ; do $(INSTALL_DATA) $$f @prefix@/VMUSBSkel ; done
	$(INSTALL_DATA) MakefileSkel     @prefix@/VMUSBSkel/Makefile
	$(mkinstalldirs) @prefix@/TclLibs/vmusb
	$(INSTALL_DATA) @srcdir@/spectclSetup.tcl @srcdir@/configFile.tcl @srcdir@/constants.tcl  @prefix@/TclLibs/vmusb
	echo pkg_mkIndex -verbose @prefix@/TclLibs/vmusb "*.tcl" | @TCLSH@
	$(mkinstalldirs) @prefix@/share/vmusb
	for f in @srcdir@/*.html ; do $(INSTALL_DATA) $$f @prefix@/share/vmusb ; done
	for f in @srcdir@/*.pdf ; do $(INSTALL_DATA)  $$f  @prefix@/share/vmusb ; done
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

