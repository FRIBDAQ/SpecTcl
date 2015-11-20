ACLOCAL_AMFLAGS=-I m4

SUBDIRS=SpecTclPackage

if HAVE_CUCUMBER
installcheck-local:
	 TCLLIBPATH=@prefix@ cucumber --format progress --guess


endif

EXTRA_DIST=features libtcl
