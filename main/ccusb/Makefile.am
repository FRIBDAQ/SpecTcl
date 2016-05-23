bin_PROGRAMS = CCUSBSpecTcl

CCUSBSpecTcl_SOURCES = CC1205Unpacker.cpp \
	CCCUSBPacket.cpp \
	CCUSBUnpacker.cpp \
	CFixedSizedUnpacker.cpp \
	CPh7xxUnpacker.cpp \
	MySpecTclApp.cpp \
	ParamMapCommand.cpp 

noinst_HEADERS = CC1205Unpacker.h \
	CCCUSBPacket.h \
	CCUSBUnpacker.h \
	CFixedSizedUnpacker.h \
	CPh7xxUnpacker.h \
	MySpecTclApp.h \
	ParamMapCommand.h


AM_CPPFLAGS = -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/Tape \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@

CCUSBSpecTcl_LDFLAGS = 	@top_builddir@/Core/libTclGrammerApp.la \
	@top_builddir@/Tape/libtape.la \
	@top_builddir@/Replace/libreplace.la \
	@top_builddir@/calibratedparams/libCalibrations.la \
	@top_builddir@/Display/libXamine.la          \
	@LIBTCLPLUS_LDFLAGS@	\
	-lXt -lX11 @X11EXTRA_LDFLAGS@  @LD_EXTRA_FLAGS@ \
	-lgsl -lgslcblas	\
	 @TK_LIBS@ @TCL_LIBS@ @XLIBSW@ \
	 -g -Wl,"-rpath=@libdir@"


#
#  This stuff installs the code in the CCUSBSkel directory so that
#  people can use it as a starting point.  For now, only the
#  SpecTclRC.tcl script is pushed there as we're going to put the
#  other scripts where they can be found in TclLibs.
#
install-exec-local:	docs_built
	$(mkinstalldirs) @prefix@/CCUSBSkel @prefix@/share/ccusb 
	$(INSTALL_DATA) @srcdir@/*.cpp @prefix@/CCUSBSkel
	$(INSTALL_DATA) @srcdir@/*.h   @prefix@/CCUSBSkel
	$(INSTALL_DATA) @srcdir@/SpecTclRC.tcl @prefix@/CCUSBSkel
	$(INSTALL_DATA) @builddir@/MakefileSkel @prefix@/CCUSBSkel/Makefile
	$(mkinstalldirs) @prefix@/TclLibs/ccusb
	$(INSTALL_DATA)  @srcdir@/spectclconfig.tcl @prefix@/TclLibs/ccusb
	echo pkg_mkIndex @prefix@/TclLibs/ccusb @prefix@/TclLibs/ccusb/spectclconfig.tcl | @TCLSH@
	$(mkinstalldirs) @prefix@/share/ccusb
	$(INSTALL_DATA)  @builddir@/*.html @prefix@/share/ccusb
	$(INSTALL_DATA)  @builddir@/*.pdf  @prefix@/share/ccusb
	-for s in {1..8} ; do		\
		$(mkinstalldirs) @prefix@/share/man/man$$s; \
		for f in *.$$s*; do  \
			$(INSTALL_DATA) $$f @prefix@/share/man/man$$s; \
		done \
	done


EXTRA_DIST=MakefileSkel.in \
	spectclconfig.tcl  \
	SpecTclRC.tcl ccusbSpecTcl.xml

docs_built: @srcdir@/ccusbSpecTcl.xml
	-@MANDOCBOOK@ man $< 2>/dev/null
	@HTMLDOCBOOK@ $<
	@HCDOCBOOK@ $<
	dvipdf       @builddir@/ccusbSpecTcl.dvi
	touch docs_built


