SUBDIRS =  Replace Scripts Utility Display \
	NSCLException Xamine Tape EventSource \
	TCL Events Gates Sorter SpectrumIO Filter SpecTcl \
	contrib

# Gri TestFiles need to be figured out still..

AUX_DIST = $(ac_aux_dir)/config.guess \
	$(ac_aux_dir)/config.sub \
	$(ac_aux_dir)/install-sh \
	$(ac_aux_dir)/ltmain.sh \
	$(ac_aux_dir)/missing \
	$(ac_aux_dir)/mkinstalldirs \
	$(ac_aux_dir)/ylwrap  \
	fix_libtool

EXTRA_DIST = fixdos ReleaseNotesNSCL4.0.txt SpecTclRC.tcl bootstrap Gri doc \
	build.cygwin CHANGELOG

dist-hook:
	(cd $(distdir)/Gri; make clean)

install-exec-local:
	install  -d   -m 0775 $(prefix) $(prefix)/bin $(prefix)/include
	install  -m 0664 config.h $(prefix)/include
	install  -d   -m 0775 $(prefix)/lib $(prefix)/etc
	$(LN_S)   $(prefix)/etc $(prefix)/Etc
	install -d   -m 0775 $(prefix)/Script $(prefix)/Skel
	install  -d   -m 0775 $(prefix)/contrib $(prefix)/doc $(prefix)/share
	install -m 0664 config.h $(prefix)/include
	rm -f $(prefix)/Bin
	rm -f $(prefix)/Lib
	rm -f $(prefix)/Include
	ln -s $(prefix)/bin $(prefix)/Bin
	ln -s $(prefix)/lib $(prefix)/Lib
	ln -s $(prefix)/include $(prefix)/Include
	tar czf - doc |(cd $(prefix); tar xzf -)
	echo "Version: " $(VERSION) build on `hostname` `date` > $(prefix)/VERSION
	$(INSTALL_DATA) CHANGELOG $(prefix)

