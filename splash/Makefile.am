install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) $(prefix)/TclLibs/splash
	$(INSTALL_SCRIPT) *.tcl $(prefix)/TclLibs/splash
	$(INSTALL_DATA)  license.terms $(prefix)/splash.license

EXTRA_DIST = pkgIndex.tcl splash.tcl

