initLDFLAGS=@TCL_LDFLAGS@



lib_LTLIBRARIES=libMapValues.la


libMapValues_la_CPPFLAGS=$(ROOT_CFLAGS) -I. 		\
	@LIBTCLPLUS_CFLAGS@					\
	@TCL_CPPFLAGS@ @ROOT_CFLAGS@ -I@top_srcdir@/Core \
	-I@top_srcdir@/Utility -I@top_srcdir@/factories

libMapValues_la_LDFLAGS = -Wl,"-rpath-link=$(libdir)" @ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@

libMapValues_la_SOURCES =	CMapValueCommand.cpp	\
				CMapValueProcessor.cpp	\
				MapValuePackage.cpp

noinst_HEADERS	=		CMapValueCommand.h	\
				CMapValueProcessor.h

install-exec-local: mapvalues.pdf
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) $(prefix)/TclLibs/mapvalues
	$(mkinstalldirs) @datarootdir@/mapvalues-plugin
	$(INSTALL_PROGRAM) .libs/libMapValues.* $(prefix)/TclLibs/mapvalues
	echo "package ifneeded MapValues 1.0 [list load [file join \$$dir libMapValues.so]]" >> 				\
	$(prefix)/TclLibs/mapvalues/pkgIndex.tcl
	$(INSTALL_DATA) @builddir@/mapvalues.pdf @datarootdir@/mapvalues-plugin

EXTRA_DIST=mapvalues.xml


mapvalues.pdf: mapvalues.xml
	$(PDFDOCBOOK) @srcdir@/mapvalues.xml



