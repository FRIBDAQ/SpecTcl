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


# Removed 	-I@top_srcdir@/Tape 


AM_CPPFLAGS = -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core \
	-I@top_srcdir@/Display \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@

# removed 	@top_builddir@/Tape/libtape.la 


CCUSBSpecTcl_LDFLAGS = 	@top_builddir@/Core/libTclGrammerApp.la \
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
install-exec-local:	
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
	$(mkinstalldirs) @prefix@/share/man/man1; 
	for f in *.1*; do  \
		$(INSTALL_DATA) $$f @prefix@/share/man/man$$s; \
	done


EXTRA_DIST=MakefileSkel.in \
	spectclconfig.tcl  \
	SpecTclRC.tcl ccusbSpecTcl.xml

BUILT_SOURCES = @builddir@/docs_built

@builddir@/docs_built: @srcdir@/ccusbSpecTcl.xml
	@MANDOCBOOK@ man $< -o @builddir@ 2>/dev/null
	@HTMLDOCBOOK@ $< -o @builddir@
	@HCDOCBOOK@ $< -o @builddir@
	dvipdf  @builddir@/ccusbSpecTcl.dvi @builddir@/ccusbSpecTcl.pdf
	touch @builddir@/docs_built

clean-local :
	rm -f @builddir@/docs_built;
