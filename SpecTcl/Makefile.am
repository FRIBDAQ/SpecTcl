SUBDIRS = Replace Scripts Utility Display Exception Xamine Tape EventSource \
	TCL Events Gates Sorter SpectrumIO SpecTcl TestFiles contrib doc

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
