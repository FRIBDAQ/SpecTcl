install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) $(prefix)/TclLibs/splash
	$(INSTALL_SCRIPT) @srcdir@/*.tcl $(prefix)/TclLibs/splash
	$(INSTALL_DATA)  @srcdir@/license.terms $(prefix)/splash.license

EXTRA_DIST = pkgIndex.tcl splash.tcl license.terms

