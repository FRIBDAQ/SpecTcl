# SpecTclGui.tcl
# top level of SpecTcl Tcl/Tk GUI interface
# Author: D. Bazin
# Date: Aug 2001

source SpectrumGenerator.tcl
source ParameterGenerator.tcl

toplevel .theTop
wm title .theTop "SpecTcl Graphical User Interface"
SetupSpectrumGenerator
SetupParameterGenerator
