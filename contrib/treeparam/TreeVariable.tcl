# TreeVariable.tcl

proc UpdateTreeVariable {variable} {
	global treeVariableRoot treeVariableName treeVariableValue treeVariableUnit
	SendMessage "treevariable -list $variable"
	set v [lindex [GetResponse] 0]
	set theName [lindex $v 0]
	set theValue [lindex $v 1]
	set theUnit [lindex $v 2]
	if {[string compare $theUnit ""] == 0} {
		set theUnit a.u.
	}
	set thePath [split $theName .]
	set theLength [llength $thePath]
	set Name ""
	for {set j 0} {$j < [expr $theLength-1]} {incr j} {
		append Name [lindex $thePath $j] .
	}
	set Name [string trimright $Name .]
	set theIndex [lsearch $treeVariableName($Name) [lindex $thePath end]]
	puts $theIndex
	set treeVariableValue($Name) [lreplace $treeVariableValue($Name) $theIndex $theIndex $theValue]
	set treeVariableUnit($Name) [lreplace $treeVariableUnit($Name) $theIndex $theIndex $theUnit]
}

proc UpdateTreeVariables {} {
# This procedure stuffs the various array variables from the treevariable names
	global treeVariableRoot treeVariableName treeVariableValue treeVariableUnit
	SendMessage "treevariable -list"
	set theList [GetResponse]
	foreach e $theList {
		set theName [lindex $e 0]
		set theValue [lindex $e 1]
		set theUnit [lindex $e 2]
		if {[string compare $theUnit ""] == 0} {
			set theUnit a.u.
		}
		set thePath [split $theName .]
		set theLength [llength $thePath]
		set rootName [lindex $thePath 0]
# Add parameter root name if it hasn't been registered yet
		if {[info exist treeVariableRoot]} {
			if {[lsearch $treeVariableRoot $rootName] == -1} {
				append treeVariableRoot "$rootName "
			}
		} else {
			set treeVariableRoot "$rootName "
		}
# Fill array variables
		for {set i 0} {$i < [expr $theLength-1]} {incr i} {
			set Name ""
			for {set j 0} {$j <= $i} {incr j} {
				append Name [lindex $thePath $j] .
			}
			set Name [string trimright $Name .]
			set subName [lindex $thePath [expr $i+1]]
			if {[info exist treeVariableName($Name)]} {
				if {[set Index [lsearch $treeVariableName($Name) $subName]] == -1} {
					append treeVariableName($Name) "$subName "
					if {$i == [expr $theLength-2]} {
						append treeVariableValue($Name) "$theValue "
						append treeVariableUnit($Name) "$theUnit "
					} else {
						append treeVariableValue($Name) "none "
						append treeVariableUnit($Name) "none "
					}
				} else {
					if {$i == [expr $theLength-2]} {
						set treeVariableValue($Name) [lreplace $treeVariableValue($Name) $Index $Index "$theValue "]
						set treeVariableUnit($Name) [lreplace $treeVariableUnit($Name) $Index $Index "$theUnit "]
					}
				}
			} else {
				set treeVariableName($Name) "$subName "
				if {$i == [expr $theLength-2]} {
					set treeVariableValue($Name) "$theValue "
					set treeVariableUnit($Name) "$theUnit "
				} else {
					set treeVariableValue($Name) "none "
					set treeVariableUnit($Name) "none "
				}
			}
		}
	}
}

proc GenerateTreeVariableMenu {parent command} {
# Generate tree variable menu and attach it to the parent widget
	global treeVariableRoot treeVariableName
	destroy $parent.root
	menu "$parent.root" -tearoff 0
	foreach e $treeVariableRoot {
		GenerateTreeVariableMenus $e $e "$parent.root" $command
		"$parent.root" add cascade -label $e -menu "$parent.root.$e"
	}
	bind "$parent.root" <Motion> "%W postcascade @%y"		
	$parent configure -menu "$parent.root"
}

proc GenerateTreeVariableMenus {member item parent command} {
# Recursive procedure to create menus
	global treeVariableName
	menu "$parent.$item" -tearoff 0
	foreach entry $treeVariableName($member) {
# Does this entry have a submenu?
		if {[info exist treeVariableName($member.$entry)] == 0} {
# No. Set it as a regular menu item
			set str "$member.$entry"
			"$parent.$item" add command -label $entry -command "$command $str"
		} else {
# Yes. Recursively create the submenu and cascade it to the item
			GenerateTreeVariableMenus "$member.$entry" $entry "$parent.$item" $command
			"$parent.$item" add cascade -label $entry \
			-menu "$parent.$item.$entry"
		}
	}
# Allows to cascade submenus without having to click on the item
	bind "$parent.$item" <Motion> "%W postcascade @%y"
}	
