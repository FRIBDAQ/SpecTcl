install-exec-local:
	$(MKDIR_P)  @prefix@/TclLibs/scalerwrapper
	$(INSTALL_DATA) @srcdir@/ScalerWrapper.tcl @prefix@/TclLibs/scalerwrapper
	echo package ifneeded ScalerWrapper 1.0 [list source [file join \$$dir ScalerWrapper.tcl]] >@prefix@/TclLibs/scalerwrapper/pkgIndex.tcl


EXTRA_DIST=ScalerWrapper.tcl
