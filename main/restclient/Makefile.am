EXTRA_DIST restclien.tcl SpecTclRestCommands.tcl  restclientdocs.xml


install-exec-hook:
	$(MKDIR_P) @prefix@/TclLibs/restclient
	$(INSTALL_SCRIPT) @srcdir@/restclient.tcl @prefix@/TclLibs/restclient
	$(INSTALL_SCRIPT) @srcdir@/SpecTclRestCommands.tcl \
			@prefix@/TclLibs/restclient
	echo pkg_mkIndex @prefix@/TclLibs/restclient "*.tcl" | @TCLSH@
	$(MKDIR_P) @prefix@/share/restclient
	$(HTMLDOCBOOK) -o @prefix@/share/restclient @srcdir@/restclientdocs.xml
