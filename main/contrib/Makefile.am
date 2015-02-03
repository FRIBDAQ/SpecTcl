CONTRIBDIR = $(srcdir)
CONTRIBDEST = $(prefix)/contrib

install-exec-local:
	$(mkinstalldirs) $(CONTRIBDEST)
	$(mkinstalldirs) $(CONTRIBDEST)/treeparam
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/treeparam/*.tcl $(CONTRIBDEST)/treeparam
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/treeparam/*.h $(CONTRIBDEST)/treeparam
	$(mkinstalldirs) $(CONTRIBDEST)/multiread
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/multiread/*.tcl $(CONTRIBDEST)/multiread
	$(mkinstalldirs) $(CONTRIBDEST)/Grace
	$(INSTALL_PROGRAM) $(CONTRIBDIR)/Grace/*.tcl $(CONTRIBDEST)/Grace


EXTRA_DIST = multiread/AAREADME.txt multiread/selectscript.tcl treeparam/*.tcl \
		treeparam/*.h Grace/*.tcl

SUBDIRS=scriptable

check-TESTS:
	- for d in $(SUBDIRS); do (cd $$d; make check-TESTS); done