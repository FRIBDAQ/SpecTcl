SUBDIRS =  Replace Scripts Utility Display Exception Xamine Tape EventSource \
	TCL Events Gates Sorter SpectrumIO Filter SpecTcl \
 	 contrib doc

# Gri TestFiles need to be figured out still..

AUX_DIST = $(ac_aux_dir)/config.guess \
	$(ac_aux_dir)/config.sub \
	$(ac_aux_dir)/install-sh \
	$(ac_aux_dir)/ltmain.sh \
	$(ac_aux_dir)/missing \
	$(ac_aux_dir)/mkinstalldirs \
	$(ac_aux_dir)/ylwrap

EXTRA_DIST = fixdos ReleaseNotesNSCL4.0.txt SpecTclRC.tcl bootstrap

dist-hook:
	(cd $(distdir) && mkdir $(ac_aux_dir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	  cp $$file $(distdir)/$$file; \
	done

install-exe-local:
	rm -f $(prefix)/Bin
	rm -f $(prefix)/Lib
	rm -f $(prefix)/Include
	ln -s $(prefix)/bin $(prefix)/Bin
	ln -s $(prefix)/lib $(prefix)/Lib
	ln -s $(prefix)/include $(prefix)/Include
