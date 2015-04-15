TCLPATH=@prefix@/TclLibs/daqdefs

install-exec-local:
	$(mkinstalldirs) @prefix@/include
	$(mkinstalldirs) $(TCLPATH)
	$(INSTALL_DATA) daqdefs.h @prefix@/include
	$(INSTALL_SCRIPT) daqdefs.tcl $(TCLPATH)
	echo pkg_mkIndex  $(TCLPATH)  "*.tcl" | @TCLSH@
