lib_LTLIBRARIES=libSpectcl.la

libSpectcl_la_SOURCES=Spectclpackage.cpp

libSpectcl_la_CXXFLAGS=-I@top_srcdir@/Core \
		@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ \
		-DINSTALLED_IN='"@prefix@"'

libSpectcl_la_LIBADD=@top_builddir@/Core/libTclGrammerApp.la   \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ 
