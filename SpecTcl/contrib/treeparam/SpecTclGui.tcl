# SpecTclGui.tcl
# top level of SpecTcl Tcl/Tk GUI interface
# Author: D. Bazin
# Date: Aug 2001 - Sept 2002
# Version 1.2 for SpecTcl 2.1: November 2003

proc CheckVersion {} {
	set version [treeparameter -version]
	if {[string compare $version "1.2"] == 0} {
		return
	}
	InformMessage $version
	exit
}

proc InformMessage {v} {
	text .thetext
	.thetext configure -font "Times -14"
	.thetext insert end "This GUI can only function with version 1.2 of the TreeParameter classes.\n"
	.thetext insert end "Your program was compiled with version $v of the TreeParameter classes.\n"
	.thetext insert end "Please make sure you are using SpecTcl version 2.1 and \
								are including the correct TreeParameter.h and TreeVariable.h files.\n"
	.thetext insert end "The default path for these files is the following:\n"
	.thetext insert end "<../contrib/treeparam/TreeParameter.h>\n"
	.thetext insert end "<../contrib/treeparam/TreeVariable.h>\n\n"
	.thetext insert end "D. Bazin - November 2003"
	pack .thetext -expand 1 -fill both
	tk_messageBox -message "Acknowledge the message"
}

proc CheckFiles {} {
	global SpecTclHome
	set fileList "\
	mclistbox.tcl \
	notebook.tcl \
	tabnbook.tcl \
	TreeParameter.tcl \
	SpectrumGenerator.tcl \
	ParameterManipulator.tcl \
	TreeVariable.tcl \
	VariableManipulator.tcl \
	GateGenerator.tcl \
	"
	foreach f $fileList {
		if {[file exists $SpecTclHome/contrib/treeparam/$f] == 0} {
			tk_messageBox -message "Incorrect TreeParameter installation. \
File $f is missing in directory $SpecTclHome/contrib/treeparam"
			exit
		}
	}
}


source $SpecTclHome/contrib/treeparam/mclistbox.tcl
source $SpecTclHome/contrib/treeparam/notebook.tcl
source $SpecTclHome/contrib/treeparam/tabnbook.tcl

source $SpecTclHome/contrib/treeparam/TreeParameter.tcl

source $SpecTclHome/contrib/treeparam/SpectrumGenerator.tcl

source $SpecTclHome/contrib/treeparam/ParameterManipulator.tcl

source $SpecTclHome/contrib/treeparam/TreeVariable.tcl

source $SpecTclHome/contrib/treeparam/VariableManipulator.tcl

source $SpecTclHome/contrib/treeparam/GateGenerator.tcl

CheckVersion

CheckFiles

toplevel .gui
wm title .gui "SpecTcl 2.1 Graphical User Interface"
tabnotebook_create .gui.main
pack .gui.main -expand 1 -fill both
SetupSpectrumGenerator .gui.main
SetupParameterManipulator .gui.main
SetupVariableManipulator .gui.main
SetupGateGenerator .gui.main
tabnotebook_display .gui.main Gates
tabnotebook_display .gui.main Variables
tabnotebook_display .gui.main Parameters
tabnotebook_display .gui.main Spectra
foreach v [treevariable -list] {
	set vName [lindex [lindex $v 0] 0]
	trace variable $vName w SetChanged
}
global spectrumMask gateMask
trace variable spectrumMask w DynamicSpectrumList
trace variable gateMask w DynamicGateList
puts "SpecTcl GUI loaded."


