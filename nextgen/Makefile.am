ACLOCAL_AMFLAGS=-I m4
SUBDIRS=Utility NSCLException TclPlus primitives \
	shell

#
#  Need to build pkgIndex.tcl
#
install-exec-local:
	echo 	package ifneeded SpecTcl 1.0 \
		[list load [file join \$$dir libSpecTclExperiment.so]] \
		> @libdir@/pkgIndex.tcl

