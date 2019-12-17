install-exec-hook:
	$(mkinstalldirs) @prefix@/TclLibs/dbconfig
	$(INSTALL_SCRIPT) dbconfig.tcl @prefix@/TclLibs/dbconfig
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig dbconfig.tcl|@TCLSH@ 
