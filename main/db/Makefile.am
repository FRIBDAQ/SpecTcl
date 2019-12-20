install-exec-hook:
	$(mkinstalldirs) @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) @srcdir@/dbconfig.tcl @prefix@/TclLibs/dbconfig
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig dbconfig.tcl|@TCLSH@ 

check-TESTS:
	(cd @srcdir@; tclsh *.test)

check: check-TESTS

EXTRA_DIST=names.test dbconfig.tcl
