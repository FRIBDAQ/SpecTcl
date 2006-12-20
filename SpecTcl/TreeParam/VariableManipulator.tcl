# VariableManipulator.tcl
# Author: D. Bazin
# Date: September 2002
# Version 1.2 - November 2003

proc SetupVariableManipulator {parent} {
	global vdefFile fullvdefFile vdefDirectory
	UpdateTreeVariables
	CreateVariableManipulator $parent
	set vdefDirectory [pwd]
	set vdefFile ""
	set fullvdefFile ""
}

proc CreateVariableManipulator {parent} {
	global topv vdefFile variable
	set topv [tabnotebook_page $parent Variables]
	
	set mod $topv.modify
	set modcolor lightgreen
	set variable(select) 1
	frame $mod -borderwidth 2 -relief groove -background $modcolor

	menubutton $mod.variable -width 5 -text Variable -bg $modcolor
	GenerateTreeVariableMenu $mod.variable SetSelectedVariable
	label $mod.labelname -text Name -background $modcolor
	label $mod.labelvalue -text Value -background $modcolor
	label $mod.labelunit -text Unit -background $modcolor
	checkbutton $mod.array -text "Array" -variable variable(Array) -background $modcolor
	grid $mod.variable $mod.labelname $mod.labelvalue $mod.labelunit x $mod.array -sticky news

	for {set i 1} {$i <= 20} {incr i} {
		VariableManipulator $i
	}

	grid columnconfigure $mod {0 1 2 3 4 5} -weight 1
	pack $mod -expand 1 -fill both

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

proc SetSelectedVariable {name} {
	global variable
	set id $variable(select)
	set variable(Name$id) $name
}

proc VariableManipulator {id} {
	global topv variable
	set mod $topv.modify
	set modcolor lightgreen

#	menubutton $mod.variable$id -width 8 -text Variable -background $modcolor
#	GenerateTreeVariableMenu $mod.variable$id "MenuLoadVariable $id"
#	GenerateTreeVariableMenu $mod.variable$id "set variable(Name$id)"
	radiobutton $mod.select$id -text "" -variable variable(select) -value $id -width 5 -bg $modcolor
	entry $mod.labelvariable$id -width 25 -textvariable variable(Name$id) -background $modcolor
	entry $mod.value$id -width 8 -textvariable variable(Value$id) -background $modcolor
	entry $mod.unit$id -width 6 -textvariable variable(Unit$id) -background $modcolor
	button $mod.load$id -width 4 -text Load -command "LoadVariable $id" -background $modcolor
	button $mod.set$id -width 4 -text Set -command "SetVariable $id" -background $modcolor
	grid $mod.select$id $mod.labelvariable$id $mod.value$id $mod.unit$id $mod.load$id $mod.set$id -sticky news
}

proc SetVariable {id} {
	global variable
	if {$variable(Array) && [IsVariableArray $id]} {
		foreach v [ListVariableArray $id] {
			treevariable -set $v $variable(Value$id) $variable(Unit$id)
		}
		UpdateTreeVariables
	} else {
		treevariable -set $variable(Name$id) $variable(Value$id) $variable(Unit$id)
		UpdateTreeVariable $variable(Name$id)
	}
	Modified
}

proc IsVariableArray {id} {
	global variable
	set suffix [lindex [split $variable(Name$id) .] end]
	return [string is integer $suffix]
}

proc ListVariableArray {id} {
	global variable
	set theList [treevariable -list]
	set suffix [lindex [split $variable(Name$id) .] end]
	set prefix [string trimright $variable(Name$id) .$suffix]
	append prefix *
	set vList ""
	foreach v $theList {
		set name [lindex $v 0]
		if {[string match $prefix $name]} {
			lappend vList $name
		}
	}
	return $vList
}

proc LoadVariable {id} {
	global variable
	set var $variable(Name$id)
	set v [lindex [treevariable -list $var] 0]
	set variable(Value$id) [lindex $v 1]
	set variable(Unit$id) [lindex $v 2]
}

proc MenuLoadVariable {id v1 v2 v3} {
	global variable
	set v [lindex [treevariable -list $variable(Name$id)] 0]
	if {[llength $v] == 0} {
		set variable(Value$id) ???
		set variable(Unit$id) ???
		return
	}
	set variable(Value$id) [lindex $v 1]
	set variable(Unit$id) [lindex $v 2]
}

#proc MenuLoadVariable {id var} {
#	global variable
#	set v [lindex [treevariable -list $var] 0]
#	set variable(Name$id) [lindex $v 0]
#	set variable(Value$id) [lindex $v 1]
#	set variable(Unit$id) [lindex $v 2]
#}

proc SaveVariableDefinition {} {
	global vdefFile fullvdefFile vdefDirectory
	set fullvdefFile [tk_getSaveFile -filetypes {{"Variable definition file" {.vdef .tcl}}} \
	-initialdir $vdefDirectory -initialfile $vdefFile -defaultextension .vdef \
	-title "Save Variable Definition file" -parent .gui]
	if {[string match $fullvdefFile ""]} {return}
	set vdefFile [lindex [split $fullvdefFile /] end]
	set vdefDirectory [string trimright $fullvdefFile $vdefFile]
	set handle [open $fullvdefFile w]
	set theList [treevariable -list]
	foreach v $theList {
		puts $handle "treevariable -set $v"
	}
	close $handle
}

proc LoadVariableDefinition {} {
	global vdefFile fullvdefFile vdefDirectory
	set fullvdefFile [tk_getOpenFile -filetypes {{"Variable definition file" {.vdef .tcl}}} \
	-initialdir $vdefDirectory -title "Load Variable Definition file" -parent .gui]
	if {[string match $fullvdefFile ""]} {return}
	set vdefFile [lindex [split $fullvdefFile /] end]
	set vdefDirectory [string trimright $fullvdefFile $vdefFile]
	source $fullvdefFile
	UpdateTreeVariables
}

proc SetChanged {name1 name2 op} {
	treevariable -setchanged $name1
}
