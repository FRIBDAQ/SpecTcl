# the line below eliminates warnings about the stuff we're dragging in from VMUSBSpecTcl

AUTOMAKE_OPTIONS = subdir-objects


lib_LTLIBRARIES=libMVLCSpecTcl.la

COMMON_cppflags = -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core  \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/factories    \
	-I@top_srcdir@/mpi \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@ @ROOT_CFLAGS@ -I@top_srcdir@/mpi \
	-I@srcdir@/../vmusb -DMVLC

# Note we use the VMUSB unpackers in place but
# They may wind up with #ifdefery for the MVLC
# definition which is done in the COMMON_cppflags
# above if code differences are needed.

VMUSB=@abs_top_srcdir@/vmusb

libMVLCSpecTcl_la_SOURCES= \
	$(VMUSB)/CParamMapCommand.cpp \
	$(VMUSB)/CStackMapCommand.cpp \
	$(VMUSB)/CModuleUnpacker.cpp \
	$(VMUSB)/C785Unpacker.cpp   \
	$(VMUSB)/CMADC32Unpacker.cpp \
	$(VMUSB)/CMDPP32QDCUnpacker.cpp \
	$(VMUSB)/CMDPP32SCPSROSoftTriggerUnpacker.cpp \
	$(VMUSB)/CMDPP32SCPSROUnpacker.cpp \
	$(VMUSB)/CMQDC32Unpacker.cpp \
	$(VMUSB)/CMTDC32Unpacker.cpp \
	$(VMUSB)/CNADC2530Unpacker.cpp \
	$(VMUSB)/CV1729Unpacker.cpp \
	$(VMUSB)/CV1x90Unpacker.cpp \
	$(VMUSB)/CV977Unpacker.cpp


libMVLCSpecTcl_la_CPPFLAGS=$(COMMON_cppflags)

bin_PROGRAMS = MVLCSpecTcl
MVLCSpecTcl_SOURCES = MySpecTclApp.cpp CStackUnpacker.cpp
noinst_HEADERS = MySpecTclApp.h CStackUnpacker.h

MVLCSpecTcl_CPPFLAGS=$(COMMON_cppflags)
MVLCSpecTcl_LDADD= @builddir@/libMVLCSpecTcl.la @top_builddir@/Core/libSpecTclMain.la \
		@top_builddir@/Core/libTclGrammerApp.la \
		    @top_builddir@/calibratedparams/libCalibrations.la \
		    @top_builddir@/Display/libXamine.la \
			@top_builddir@/mpi/libMPIUtilities.la \
	-lXt -lX11 @X11EXTRA_LDFLAGS@  @LD_EXTRA_FLAGS@ @ROOT_LDFLAGS@ \
	-lgsl -lgslcblas	\
	 @TK_LIBS@ @TCL_LIBS@ @XLIBSW@

MVLCSpecTcl_LDFLAGS = -g -Wl,"-rpath=@libdir@"


## Install a MVLCSkel so users can extend this.

install-exec-local:
	$(mkinstalldirs) @prefix@/MVLCSkel
	for f in $(MVLCSpecTcl_SOURCES) $(noinst_HEADERS); do $(INSTALL_DATA) @srcdir@//$$f @prefix@/MVLCSkel; done
	$(INSTALL_SCRIPT) $(VMUSB)/SpecTclRC.tcl @prefix@/MVLCSkel
	$(INSTALL_SCRIPT) $(VMUSB)/SpecTclInit.tcl @prefix@/MVLCSkel

