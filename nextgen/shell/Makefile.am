bin_PROGRAMS=spectcl

spectcl_SOURCES=spectcl.c


spectcl_CFLAGS=@TCL_FLAGS@ -DLIBDIR="\"@libdir@\""
spectcl_LDADD=@TCL_LDFLAGS@ @top_srcdir@/primitives/libSpecTclExperiment.la

EXTRA_DIST=spectcl.tcl

install-exec-local:
	$(mkinstalldirs)   @libdir@
	$(install_sh_SCRIPT) spectcl.tcl @libdir@