install-data-local:
	$(mkinstalldirs) @prefix@/TclLibs
	$(mkinstalldirs) @prefix@/TclLibs/tkcon
	$(INSTALL_SCRIPT) @srcdir@/tkcon-2.5.tcl @prefix@/TclLibs/tkcon
	$(INSTALL_SCRIPT) @srcdir@/history.tcl @prefix@/TclLibs/tkcon
	echo pkg_mkIndex @prefix@/TclLibs/tkcon "*.tcl" | @TCLSH@
	echo "package ifneeded tkcon 3.0 [list source [file join \$$dir tkcon-2.5.tcl]]" >> @prefix@/TclLibs/tkcon/pkgIndex.tcl

EXTRA_DIST=tkcon-2.5.tcl history.tcl
