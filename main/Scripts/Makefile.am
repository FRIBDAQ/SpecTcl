scriptsdir = @prefix@/Script
pkgdir     = @prefix@/TclLibs/utilities

packages   = ServicePort.tcl

scripts_SCRIPTS = dtree.tcl fileall.tcl gui.tcl handle.tcl \
	listall.tcl save.head tkcon.tcl \
	tree.tcl CopyGates.tcl

testScripts = fileall_test all.tcl dependsondeleted.test formatgatetest.test \
	gateparamcount.test generalgatetype.test \
	getdependencies.test listify.test \
	markwritable.test setstatus.test \
	validparams.test wrap.test

EXTRA_DIST = $(scripts_SCRIPTS) $(testScripts) $(packages)

install-exec-local:
	$(mkinstalldirs) $(pkgdir)
	for f in $(packages); do \
            $(INSTALL_SCRIPT) @srcdir@/$$f $(pkgdir); \
        done
	echo pkg_mkIndex -verbose  $(pkgdir) "*.tcl" | @TCLSH@


check :
	(cd @srcdir@ ; ./fileall_test )
