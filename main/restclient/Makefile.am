EXTRA_DIST= restclient.tcl SpecTclRestCommand.tcl  restclientdocs.xml


install-exec-hook:
	$(MKDIR_P) @prefix@/TclLibs/restclient
	$(INSTALL_SCRIPT) @srcdir@/restclient.tcl @prefix@/TclLibs/restclient
	$(INSTALL_SCRIPT) @srcdir@/SpecTclRestCommand.tcl \
			@prefix@/TclLibs/restclient
	echo pkg_mkIndex @prefix@/TclLibs/restclient "*.tcl" | @TCLSH@
	$(MKDIR_P) @prefix@/share/restclient
	$(HTMLDOCBOOK) -o @prefix@/share/restclient @srcdir@/restclientdocs.xml
