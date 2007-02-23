SCRIPTDEST = $(prefix)/Script
HELPDEST   = $(prefix)/Script/help
ICONDEST   = $(prefix)/Script/icons

install-exec-local:
	$(mkinstalldirs) $(SCRIPTDEST) $(HELPDEST) $(ICONDEST)
	$(INSTALL_PROGRAM) $(srcdir)/*.tcl   $(SCRIPTDEST)
	$(INSTALL_DATA)    $(srcdir)/*.html  $(HELPDEST)
	$(INSTALL_DATA)    $(srcdir)/*.gif   $(ICONDEST)


EXTRA_DIST = 2dGate.html \
Browser.html \
C2Gate.html \
Concepts.html \
Gates.html \
Intro.html \
Menus.html \
Objects.html \
Parameters.html \
Spectra.html \
Variable.html \
Vocabulary.html \
applyGateOnSpectrum.html \
applyThisGateOnSpectra.html \
applygate.tcl \
attachPipe.html \
axisicon.gif \
browser.tcl \
c2band.tcl \
compoundGate.html \
compoundgate.tcl \
dataSourceMenu.html \
datasource.tcl \
deleteMultipleGates.html \
edit1d.tcl \
edit2d.tcl \
edit2dmulti.tcl \
editParameter.html \
editPseudo.html \
editmulti.tcl \
editparameter.tcl \
editstrip.tcl \
editvariable.tcl \
fileMenu.html \
filterEnables.html \
filterMenu.html \
filtercontrol.tcl \
gate.tcl \
gateFilter.html \
gateInfo.html \
gateMenu.html \
gateicon.gif \
genericGate.html \
genericSpectrumBuilder.html \
guihelp.tcl \
guistate.tcl \
guiutilities.tcl \
hostPrompt.html \
maskGate.html \
maskeditor.tcl \
multiparam.html \
multipoint.tcl \
nameFilter.html \
newGui.tcl \
parameterInfo.html \
pkgIndex.tcl \
pointicon.gif \
pseudo.tcl \
pseudoicon.gif \
readspectrum.html \
savespectrum.html \
selectGateApplyToMultiple.html \
selectspectrumlist.html \
setFilterFile.html \
setParameters.html \
slice.tcl \
sliceGate.html \
spectraMenu.html \
spectrum.tcl \
spectrum1d.html \
spectrum2d.html \
spectrumInfo.html \
spectrumicon.gif \
statusLine.html \
stripchart.html \
treeparicon.gif \
treevaricon.gif \
variableInfo.html \
edit2dmulti.html  \
savestatedialog.html \
prefs.tcl \
editGammaDeluxe.tcl \
editGammaDeluxehelp.html
