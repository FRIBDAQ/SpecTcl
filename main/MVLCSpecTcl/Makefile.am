lib_LTLIBRARIES=libMVLCSpecTcl.la

COMMON_cppflags = -I@top_srcdir@/Utility \
	-I@top_srcdir@/Core  \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/factories    \
	-I@top_srcdir@/mpi \
	@TCL_CPPFLAGS@ -DINSTALLED_IN='"@prefix@"' @XINCLUDES@ \
	@LIBTCLPLUS_CFLAGS@ @ROOT_CFLAGS@ -I@top_srcdir@/mpi \
	-I@srcdir@/../vmusb

VMUSB=@srcdir@/../vmusb
libMVLCSpecTcl_la_SOURCES=CStackUnpacker.cpp \
	$(VMUSB)/CParamMapCommand.cpp \
	$(VMUSB)/CStackMapCommand.cpp

libMVLCSpecTcl_la_CPPFLAGS=$(COMMON_cppflags)
