
EXTRA_DIST = GateGenerator.tcl \
	ParameterGenerator.tcl \
	ParameterManipulator.tcl \
	SpecTclGui.tcl \
	SpectrumGenerator.tcl \
	TopGui.tcl \
	TreeParameter.tcl \
	TreeVariable.tcl \
	VariableManipulator.tcl \
	mclistbox.tcl \
	notebook.tcl \
	server.tcl \
	tabnbook.tcl


SCRIPTDEST = $(prefix)/Script
install-exec-local:
	$(mkinstalldirs) $(SCRIPTDEST)
	$(INSTALL_PROGRAM) *.tcl $(SCRIPTDEST)

