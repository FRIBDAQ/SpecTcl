lib_LTLIBRARIES=libSpecTclDb.la

libSpecTclDb_la_CXXFLAGS=-I@top_srcdir@/Core @SQLITE3_CFLAGS@ \
	-I@top_srcdir@/Utility @ROOT_CFLAGS@ @TCL_CPPFLAGS@ \
	@LIBTCLPLUS_CFLAGS@
libSpecTclDb_la_LDFLAGS=@SQLITE3_LDFLAGS@ @ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@
libSpecTclDb_la_SOURCES=CDBEvents.cpp DBEvents.cpp
include_HEADERS=CDBEvents.h DBEvents.h


install-exec-hook:
	$(mkinstalldirs) @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dbconfig.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dbgui.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dialogwrapper.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_DATA)   @srcdir@/*.png             @prefix@/TclLibs/dbconfig
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig "*.tcl *.so"|@TCLSH@ 

check-TESTS:
	(cd @srcdir@; tclsh *.test)

check: check-TESTS

EXTRA_DIST=names.test dbconfig.tcl dbgui.tcl dialogwrapper.tcl \
	configuration.png folder.png openfolder.png spectrum.png
