# TopGui.tcl
# top level of SpecTcl Tcl/Tk GUI interface
# Author: D. Bazin
# Date: Aug 2001 - Sept 2002

proc ServerRead {} {
	global server serverWait serverResponse
	set line [gets $server]
	if {[string compare $line Done] == 0} {
		set serverWait 0
		return
	}
	if {[string length $line] > 0} {
		append serverResponse "$line\n"
	}
}

proc SendMessage {message} {
	global server serverResponse TopTitle
	set serverResponse ""
	puts $server $message
	set TopTitle [wm title .]
	wm title . "Command in progress ..."
}

proc GetResponse {} {
	global serverResponse serverWait TopTitle
	vwait serverWait
#	puts "Getting response: $serverResponse"
	wm title . $TopTitle
	return $serverResponse
}

proc CheckVersion {} {
	SendMessage "treeparameter -version"
	set version [GetResponse]
	if {[string compare $version "1.1\n"] == 0} {
		return
	}
	InformMessage	
	exit
}

proc InformMessage {} {
	text .thetext
	.thetext configure -font "Times -14"
	.thetext insert end "This version of the SpecTcl GUI cannot function with \
your compiled version of the TreeParameter class.\n"
	.thetext insert end "Please change the path of the #include statements to the \
TreeParameter.h and TreeVariable.h files to the following:\n"
	.thetext insert end "<../contrib/treeparam/TreeParameter.h>\n"
	.thetext insert end "<../contrib/treeparam/TreeVariable.h>\n"
	.thetext insert end "in all your files where they are included, and remove\
any local version of these files.\n"
	.thetext insert end "Then recompile SpecTcl.\n\n"
	.thetext insert end "D. Bazin - October 2002"
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
	ParameterGenerator.tcl \
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

set server [socket $env(HOST) 9111]
fconfigure $server -buffering line -blocking 0 -buffersize 65536
fileevent $server readable ServerRead

source $SpecTclHome/contrib/treeparam/mclistbox.tcl
source $SpecTclHome/contrib/treeparam/notebook.tcl
source $SpecTclHome/contrib/treeparam/tabnbook.tcl

source $SpecTclHome/contrib/treeparam/TreeParameter.tcl
source $SpecTclHome/contrib/treeparam/SpectrumGenerator.tcl
source $SpecTclHome/contrib/treeparam/ParameterGenerator.tcl
source $SpecTclHome/contrib/treeparam/TreeVariable.tcl
source $SpecTclHome/contrib/treeparam/VariableManipulator.tcl
source $SpecTclHome/contrib/treeparam/GateGenerator.tcl
#source $SpecTclHome/contrib/treeparam/Interface.tcl

CheckVersion
CheckFiles
wm title . "SpecTcl Graphical User Interface"
tabnotebook_create .main
pack .main -expand 1 -fill both
SetupSpectrumGenerator .main
SetupParameterGenerator .main
SetupVariableManipulator .main
SetupGateGenerator .main
#SetupInterface .main
#tabnotebook_display .main Interface
tabnotebook_display .main Gates
tabnotebook_display .main Variables
tabnotebook_display .main Parameters
tabnotebook_display .main Spectra
