include_HEADERS =  NamedItem.h buffer.h buftypes.h Refptr.h daqtypes.h \
	histotypes.h SpecMath.h DesignByContract.h Fstream.h Iomanip.h Iostream.h \
	Istream.h Ostream.h Sstream.h Typeinfo.h fdstream.h daqdatatypes.h

noinst_HEADERS = Asserts.h

TCLPATH=@prefix@/TclLibs/daqdefs

install-exec-local:
	$(mkinstalldirs) @prefix@/include
	$(mkinstalldirs) $(TCLPATH)
	$(INSTALL_DATA) daqdefs.h @prefix@/include
	$(INSTALL_SCRIPT) daqdefs.tcl $(TCLPATH)
	echo pkg_mkIndex  $(TCLPATH)  "*.tcl" | @TCLSH@

EXTRA_DIST = Utility.WC Utility.omt
