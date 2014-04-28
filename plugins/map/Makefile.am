initLDFLAGS=@TCL_LDFLAGS@

AM_CPPFLAGS = -I. 						\
	@SPECTCLCCSW@					\
	@TCLCCSW@

lib_LTLIBRARIES=libMapValues.la

libMapValues_la_LDFLAGS = -Wl,"-rpath-link=$(libdir)"

libMapValues_la_SOURCES =	CMapValueCommand.cpp	\
				CMapValueProcessor.cpp	\
				MapValuePackage.cpp

noinst_HEADERS	=		CMapValueCommand.h	\
				CMapValueProcessor.h

#  Install myself in $(prefix)/TclLibs/mapvalues

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) $(prefix)/TclLibs/mapvalues
	$(INSTALL_PROGRAM) .libs/libMapValues.* $(prefix)/TclLibs/mapvalues
	echo "package ifneeded MapValues 1.0 [list load [file join \$$dir libMapValues.so]]" >> 				\
	$(prefix)/TclLibs/mapvalues/pkgIndex.tcl


EXTRA_DIST=mapvalues.xml

mapvalues.dvi: mapvalues.xml
	$(DOCBOOK) $<

mapvalues.pdf: mapvalues.dvi
	$(DVIPDF) $<


docs: mapvalues.pdf
