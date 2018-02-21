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
CMTDC32Unpacker.cpp \
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
CMTDC32Unpacker.h \
MySpecTclApp.h

# Removed: 	-I@top_srcdir@/Tape 


VMUSBSpecTcl_CPPFLAGS = -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core  \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/factories    \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@

# Removed 		    @top_builddir@/Tape/libtape.la 


VMUSBSpecTcl_LDADD =@top_builddir@/Core/libTclGrammerApp.la \
		    @top_builddir@/calibratedparams/libCalibrations.la \
		    @top_builddir@/Display/libXamine.la

VMUSBSpecTcl_LDFLAGS = 	\
	@LIBTCLPLUS_LDFLAGS@	\
	-lXt -lX11 @X11EXTRA_LDFLAGS@  @LD_EXTRA_FLAGS@ \
	-lgsl -lgslcblas	\
	 @TK_LIBS@ @TCL_LIBS@ @XLIBSW@ \
	 -g -Wl,"-rpath=@libdir@"


install-exec-local:
	$(mkinstalldirs) @prefix@/VMUSBSkel @prefix@/share/vmusb
	for f in @srcdir@/*.cpp ; do $(INSTALL_DATA) $$f @prefix@/VMUSBSkel ; done
	for f in @srcdir@/*.h ; do $(INSTALL_DATA) $$f @prefix@/VMUSBSkel ; done
	$(INSTALL_DATA) MakefileSkel     @prefix@/VMUSBSkel/Makefile
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


EXTRA_DIST = MakefileSkel.in configFile.tcl spectclSetup.tcl CLLNLUnpacker.cpp  vmusb.xml constants.tcl

BUILT_SOURCES = @builddir@/docs_built



@builddir@/docs_built: @srcdir@/vmusb.xml
	@MANDOCBOOK@ man $< -o @builddir@ 2>/dev/null
	@HTMLDOCBOOK@ $< -o @builddir@
	@HCDOCBOOK@ $< -o @builddir@
	dvipdf       @builddir@/vmusb.dvi @builddir@/vmusb.pdf
	touch @builddir@/docs_built


clean-local:
	rm -f @builddir@/docs_built

