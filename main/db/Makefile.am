install-exec-hook:
	$(mkinstalldirs) @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dbconfig.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dbgui.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dialogwrapper.tcl @prefix@/TclLibs/dbconfig
	$(INSTALL_DATA)   @srcdir@/*.png             @prefix@/TclLibs/dbconfig
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig "*.tcl"|@TCLSH@ 

check-TESTS:
	(cd @srcdir@; tclsh *.test)

check: check-TESTS

EXTRA_DIST=names.test dbconfig.tcl dbgui.tcl dialogwrapper.tcl \
	configuration.png folder.png openfolder.png spectrum.png
