lib_LTLIBRARIES=libSpecTclDb.la

TCLSCRIPTS=dbconfig.tcl dbgui.tcl dialogwrapper.tcl
IMAGES=configuration.png folder.png openfolder.png spectrum.png


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
	for f in $(TCLSCRIPTS) ;  do $(INSTALL_SCRIPT) @srcdir@/$$f @prefix@/TclLibs/dbconfig; done
	for f in $(IMAGES); do $(INSTALL_DATA) @srcdir@/$$f @prefix@/TclLibs/dbconfig; done
	rm -f @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	$(LN_S)           @libdir@/libSpecTclDb.so   @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig "*.tcl"|@TCLSH@ 
	echo package ifneeded SpecTclDb 1.0 [list load [file join \$$dir libSpecTclDb.so]] >> @prefix@/TclLibs/dbconfig/pkgIndex.tcl


check-TESTS:
	(cd @srcdir@; tclsh *.test)

check: check-TESTS

EXTRA_DIST=names.test $(TCLSCRIPTS) $(IMAGES)

