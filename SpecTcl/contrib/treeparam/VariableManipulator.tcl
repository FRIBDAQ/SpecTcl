# VariableManipulator.tcl
# Author: D. Bazin
# Date: September 2002

proc SetupVariableManipulator {parent} {
	global vdefFile fullvdefFile vdefDirectory
	UpdateTreeVariables
	CreateVariableManipulator $parent
	set vdefDirectory [pwd]
	set vdefFile ""
	set fullvdefFile ""
}

proc CreateVariableManipulator {parent} {
	global topv vdefFile
	set topv [tabnotebook_page $parent Variables]
	
	VariableManipulator $parent 1
	VariableManipulator $parent 2
	VariableManipulator $parent 3
	VariableManipulator $parent 4
	VariableManipulator $parent 5
	VariableManipulator $parent 6
	VariableManipulator $parent 7
	VariableManipulator $parent 8
	VariableManipulator $parent 9
	VariableManipulator $parent 10
	
	set wname $topv.control
	frame $wname -borderwidth 2 -relief groove -background lightyellow
		button $wname.load -text Load -command LoadVariableDefinition -background lightyellow
		button $wname.save -text Save -command SaveVariableDefinition -background lightyellow
		label $wname.message -text "Current file: " -background lightyellow
		label $wname.file -width 20 -textvariable vdefFile -background lightyellow
		pack $wname.load $wname.save $wname.message $wname.file -side left -expand 1 -fill x
	pack $wname -expand 1 -fill x
	
	set wname $topv.fill
	frame $wname
	pack $wname -expand 1 -fill both
	
	pack $topv -expand 1 -fill both
}

proc VariableManipulator {parent id} {
	global topv variable
	set mod $topv.modify$id
	set modcolor lightgreen
	frame $mod -borderwidth 2 -relief groove -background $modcolor
		menubutton $mod.variable -width 8 -text Variable -background $modcolor
		GenerateTreeVariableMenu $mod.variable "MenuLoadVariable $id"
		entry $mod.labelvariable -width 25 -textvariable variable(Name$id) -background $modcolor
		entry $mod.value -width 8 -textvariable variable(Value$id) -background $modcolor
		entry $mod.unit -width 6 -textvariable variable(Unit$id) -background $modcolor
		button $mod.set -width 4 -text Set -command "SetVariable $id" -background $modcolor
		button $mod.load -width 4 -text Load -command "LoadVariable $id" -background $modcolor
		grid $mod.variable $mod.labelvariable $mod.value $mod.unit $mod.set $mod.load -sticky news
		grid columnconfigure $mod {0 1 2 3 4 5} -weight 1
	pack $mod -expand 1 -fill x
}

proc SetVariable {id} {
	global variable
	SendMessage "treevariable -set $variable(Name$id) $variable(Value$id) $variable(Unit$id)"
	GetResponse
	UpdateTreeVariable $variable(Name$id)
}

proc LoadVariable {id} {
	global variable
	set var $variable(Name$id)
	SendMessage "treevariable -list $var"
	set v [lindex [GetResponse] 0]
	set variable(Value$id) [lindex $v 1]
	set variable(Unit$id) [lindex $v 2]
}

proc MenuLoadVariable {id var} {
	global variable
	SendMessage "treevariable -list $var"
	set v [lindex [GetResponse] 0]
	set variable(Name$id) [lindex $v 0]
	set variable(Value$id) [lindex $v 1]
	set variable(Unit$id) [lindex $v 2]
}

proc SaveVariableDefinition {} {
	global vdefFile fullvdefFile vdefDirectory
	set fullvdefFile [tk_getSaveFile -filetypes {{"Variable definition file" {.vdef .tcl}}} \
	-initialdir $vdefDirectory -initialfile $vdefFile -defaultextension .vdef \
	-title "Save Variable Definition file"]
	if {[string match $fullvdefFile ""]} {return}
	set vdefFile [lindex [split $fullvdefFile /] end]
	set vdefDirectory [string trimright $fullvdefFile $vdefFile]
	set handle [open $fullvdefFile w]
	SendMessage "treevariable -list"
	set theList [GetResponse]
	foreach v $theList {
		puts $handle "treevariable -set $v"
	}
	close $handle
}

proc LoadVariableDefinition {} {
	global vdefFile fullvdefFile vdefDirectory
	set fullvdefFile [tk_getOpenFile -filetypes {{"Variable definition file" {.vdef .tcl}}} \
	-initialdir $vdefDirectory -title "Load Variable Definition file"]
	if {[string match $fullvdefFile ""]} {return}
	set vdefFile [lindex [split $fullvdefFile /] end]
	set vdefDirectory [string trimright $fullvdefFile $vdefFile]
	SendMessage "source $fullvdefFile"
	GetResponse
#	set handle [open $fullvdefFile r]
#	while {![eof $handle]} {
#		SendMessage [gets $handle]
#		GetResponse
#	}
#	close $handle
	UpdateTreeVariables
}
