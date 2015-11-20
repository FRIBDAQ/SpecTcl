# SpecTclGui.tcl
# top level of SpecTcl Tcl/Tk GUI interface
# Author: D. Bazin
# Date: Aug 2001 - Sept 2002
# Version 1.3 for SpecTcl 3.0: April 2005

set requiredVersion 2.1

proc CheckVersion {} {
    global requiredVersion
	set version [treeparameter -version]
	if {[string compare $version $requiredVersion] == 0} {
		return
	}
	InformMessage $version
	exit
}

proc InformMessage {v} {
    global requiredVersion
	text .thetext
	.thetext configure -font "Times -14"
	.thetext insert end "This GUI can only function with version $requiredVersion of the TreeParameter classes.\n"
	.thetext insert end "Your program was compiled with version $v of the TreeParameter classes.\n"
	.thetext insert end "Please make sure you are using SpecTcl version 2.1 and \
								are including the correct TreeParameter.h and TreeVariable.h files.\n"
	.thetext insert end "The default path for these files is the following:\n"
	.thetext insert end "<TreeParameter.h>\n"
	pack .thetext -expand 1 -fill both
	tk_messageBox -message "Acknowledge the message"
}

proc CheckFiles {} {
	global TreeParameterHome
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
		if {[file exists $TreeParameterHome/$f] == 0} {
			tk_messageBox -message "Incorrect TreeParameter installation. \
File $f is missing in directory $TreeParameterHome"
			exit
		}
	}
}

set answer [tk_messageBox -icon question -type yesno -title "New Gui" \
		-message {This is the old SpecTcl GUI.  If you want to use the new GUI, click "Yes" below}]
if {$answer == "yes"} {
    source $SpecTclHome/Script/newGui.tcl
} else {

    
    if {![info exist TreeParameterHome]} {
	set scriptname [info script]
	set scriptdir  [file dirname $scriptname]
	set TreeParameterHome $scriptdir
    }
    
    source $TreeParameterHome/mclistbox.tcl
    source $TreeParameterHome/notebook.tcl
    source $TreeParameterHome/tabnbook.tcl
    
    source $TreeParameterHome/TreeParameter.tcl
    
    source $TreeParameterHome/SpectrumGenerator.tcl
    
    source $TreeParameterHome/ParameterManipulator.tcl
    
    source $TreeParameterHome/TreeVariable.tcl
    
    source $TreeParameterHome/VariableManipulator.tcl
    
    source $TreeParameterHome/GateGenerator.tcl
    
    CheckVersion
    
    CheckFiles
    
    GenerateMenuBitmaps
    puts "Building SpecTcl GUI ..."
    update
    
    set version [treeparameter -version]
    
    toplevel .gui
    wm title .gui "TreeParameter GUI version $version"
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
    trace variable spectrumParameterX w SpectrumParameterXCommand
    trace variable spectrumParameterY w SpectrumParameterYCommand
    for {set i 1} {$i <= 20} {incr i} {
	trace variable parameter(Name$i) w "MenuLoadParameter $i"
	trace variable variable(Name$i) w "MenuLoadVariable $i"
    }
    foreach v [treevariable -list] {
	set vName [lindex [lindex $v 0] 0]
	trace variable $vName w SetChanged
    }
    global spectrumMask gateMask
    trace variable spectrumMask w DynamicSpectrumList
    trace variable gateMask w DynamicGateList
    puts "SpecTcl GUI loaded."
    
}
