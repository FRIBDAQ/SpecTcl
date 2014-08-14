install-data-local:
	$(mkinstalldirs) @prefix@/TclLibs
	$(mkinstalldirs) @prefix@/TclLibs/tkcon
	$(INSTALL_SCRIPT) tkcon-2.5.tcl @prefix@/TclLibs/tkcon
	$(INSTALL_SCRIPT) history.tcl @prefix@/TclLibs/tkcon
	echo pkg_mkIndex @prefix@/TclLibs/tkcon "*.tcl" | @TCLSH@

EXTRA_DIST=tkcon-2.5.tcl history.tcl
