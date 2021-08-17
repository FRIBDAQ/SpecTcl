install-exec-hook:
	$(MKDIR_P) @prefix@/TclLibs/restclient
	$(INSTALL_SCRIPT) @srcdir@/restclient.tcl @prefix@/TclLibs/restclient
	echo pkg_mkIndex @prefix@/TclLibs/restclient "*.tcl" | @TCLSH@
