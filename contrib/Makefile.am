CONTRIBDIR = $(srcdir)
CONTRIBDEST = $(prefix)/contrib

install-exec-local:
	$(mkinstalldirs) $(CONTRIBDEST)
	$(mkinstalldirs) $(CONTRIBDEST)/treeparam
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/treeparam/*.tcl $(CONTRIBDEST)/treeparam
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/treeparam/*.h $(CONTRIBDEST)/treeparam
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/treeparam/Makefile $(CONTRIBDEST)/treeparam
	$(mkinstalldirs) $(CONTRIBDEST)/multiread
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/multiread/*.tcl $(CONTRIBDEST)/multiread
	$(mkinstalldirs) $(CONTRIBDEST)/Grace
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/Grace/*.tcl $(CONTRIBDEST)/Grace


EXTRA_DIST = multiread/AAREADME.txt multiread/selectscript.tcl treeparam/*.tcl treeparam/*.h treeparam/Documentation/*.html treeparam/Documentation/pics/tree.jpg treeparam/Example/*.* treeparam/Example/Makefile treeparam/Makefile \
Grace/*.tcl

