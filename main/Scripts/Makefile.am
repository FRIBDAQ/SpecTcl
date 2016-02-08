SCRIPTDIR = $(srcdir)
SCRIPTDEST = $(prefix)/Script

install-exec-local:
	$(mkinstalldirs) $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/*.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/fileall.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/gui.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/handle.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/listall.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/save.head  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/tkcon.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/tree.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/CopyGates.tcl  $(SCRIPTDEST)

EXTRA_DIST = dtree.tcl fileall.tcl gui.tcl handle.tcl listall.tcl tkcon.tcl tree.tcl \
	CopyGates.tcl save.head all.tcl dependsondeleted.test formatgatetest.test \
	gateparamcount.test generalgatetype.test \
	getdependencies.test listify.test markwritable.test setstatus.test validparams.test \
	wrap.test

AM_TESTS_ENVIRONMENT = TCLLIBPATH='@srcdir@'; export TCLLIBPATH
TESTS = all.tcl
TCL_LOG_COMPILER = $(TCLSH)
