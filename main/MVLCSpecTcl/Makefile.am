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

VMUSB=@srcdir@/../vmusb
libMVLCSpecTcl_la_SOURCES=CStackUnpacker.cpp \
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
