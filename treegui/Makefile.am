PKGDEST    = $(prefix)/TclLibs/treeGui
SCRIPTDEST = $(prefix)/Script

install-exec-local:
	$(mkinstalldirs) $(PKGDEST)
	$(INSTALL_PROGRAM) $(srcdir)/*.tcl $(PKGDEST)
	$(INSTALL_DATA)    $(srcdir)/*.gif $(PKGDEST)

	$(INSTALL_PROGRAM) $(srcdir)/treeGui.tcl $(SCRIPTDEST)
	(cd $(PKGDEST); echo 	pkg_mkIndex . *.tcl |tclsh)


EXTRA_DIST = autosave.tcl \
definitionFile.tcl \
gateContainer.tcl \
gateCreate.tcl \
gateCreateTest.tcl \
gateDeleteBar.tcl \
gateSelBar.tcl \
gateSelTest.tcl \
gateTabActions.tcl \
gateTable.tcl \
gateTableTest.tcl \
parametersTabActions.tcl \
restore.tcl \
spectrumAxis.tcl \
spectrumContainer.tcl \
spectrumManipulation.tcl \
spectrumMask.tcl \
spectrumTabActions.tcl \
spectrumTable.tcl \
spectrumType.tcl \
treeGui.tcl \
treemenuWidget.tcl \
treeParametersContainer.tcl \
treeParameterWidget.tcl \
treeUtilities.tcl \
treeVariableContainer.tcl \
treeVariableEditor.tcl \
treeVariableLoadSave.tcl \
variableTabActions.tcl \
downarrow.gif \
uparrow.gif