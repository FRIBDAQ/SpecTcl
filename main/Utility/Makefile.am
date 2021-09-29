include_HEADERS =  NamedItem.h buffer.h buftypes.h Refptr.h daqtypes.h \
	histotypes.h SpecMath.h DesignByContract.h Fstream.h Iomanip.h Iostream.h \
	Istream.h Ostream.h Sstream.h Typeinfo.h fdstream.h daqdatatypes.h \
	TclDict.h

lib_LTLIBRARIES = libUtilities.la
libUtilities_la_SOURCES=TclDict.cpp
libUtilities_la_CXXFLAGS=@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@
libUtilities_la_LDFLAGS=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @ROOT_LDFLAGS@

noinst_HEADERS = Asserts.h

TCLPATH=@prefix@/TclLibs/daqdefs

install-exec-local:
	$(mkinstalldirs) @prefix@/include
	$(mkinstalldirs) $(TCLPATH)
	$(INSTALL_DATA) daqdefs.h @prefix@/include
	$(INSTALL_SCRIPT) daqdefs.tcl $(TCLPATH)
	echo pkg_mkIndex  $(TCLPATH)  "*.tcl" | @TCLSH@

EXTRA_DIST = Utility.WC Utility.omt
