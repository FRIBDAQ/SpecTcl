lib_LTLIBRARIES=libSpectcl.la

libSpectcl_la_SOURCES=Spectclpackage.cpp

libSpectcl_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
		@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ \
		-DINSTALLED_IN='"@prefix@"'

libSpectcl_la_LIBADD=@top_builddir@/Core/libTclGrammerApp.la   \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ 

install-exec-hook: @builddir@/libSpectcl.la
	$(mkinstalldirs) @prefix@/TclLibs/spectclpackage
	$(INSTALL_PROGRAM) @libdir@/libSpectcl.so @prefix@/TclLibs/spectclpackage
	echo package ifneeded spectcl 1.0 [list load [file join \$$dir libSpectcl.so]] > @prefix@/TclLibs/spectclpackage/pkgIndex.tcl
