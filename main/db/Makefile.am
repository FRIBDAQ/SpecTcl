lib_LTLIBRARIES=libSpecTclDb.la

libSpecTclDb_la_CXXFLAGS=-I@top_srcdir@/Core @SQLITE3_CFLAGS@ \
	-I@top_srcdir@/Utility @ROOT_CFLAGS@ @TCL_CPPFLAGS@ \
	@LIBTCLPLUS_CFLAGS@ -I@top_srcdir@/factories \
	-DTCLLIBS=@prefix@/TclLibs
libSpecTclDb_la_LDFLAGS= @builddir@/../Core/libTclGrammerApp.la   \
	@SQLITE3_LDFLAGS@ @ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@
libSpecTclDb_la_SOURCES=CDBEvents.cpp DBEvents.cpp CDBCommands.cpp CDBPackage.cpp
include_HEADERS=CDBEvents.h DBEvents.h CDBCommands.h


install-exec-hook:
	$(mkinstalldirs) @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dbconfig.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dbgui.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dialogwrapper.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_DATA)   @srcdir@/*.png             @prefix@/TclLibs/dbconfig
	rm -f @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	$(LN_S)           @libdir@/libSpecTclDb.so   @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig "*.tcl *.so"|@TCLSH@ 

check-TESTS:
	(cd @srcdir@; tclsh *.test)

check: check-TESTS

EXTRA_DIST=names.test dbconfig.tcl dbgui.tcl dialogwrapper.tcl \
	configuration.png folder.png openfolder.png spectrum.png
