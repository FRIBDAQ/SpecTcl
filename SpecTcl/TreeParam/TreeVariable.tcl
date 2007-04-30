
# TreeVariable.tcl
# Tcl/Tk script to generate a menu driven spectrum creation
# Author: D. Bazin
# Date: Dec 2000 - Sept 2002
# Version 1.2 November 2003

proc UpdateTreeVariable {variable} {
	global treeVariableRoot treeVariableName treeVariableValue treeVariableUnit
	set v [lindex [treevariable -list $variable] 0]
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
	set treeVariableValue($Name) [lreplace $treeVariableValue($Name) $theIndex $theIndex $theValue]
	set treeVariableUnit($Name) [lreplace $treeVariableUnit($Name) $theIndex $theIndex $theUnit]
}

proc UpdateTreeVariables {} {
# This procedure stuffs the various array variables from the treevariable names
	global treeVariableRoot treeVariableName treeVariableValue treeVariableUnit
	set theList [treevariable -list]
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
	menu $parent.root -tearoff 0
	foreach e $treeVariableRoot {
		GenerateTreeVariableMenus $e $e $parent.root $command
		$parent.root add cascade -label $e -menu $parent.root.$e \
		-background lightyellow -activebackground yellow
	}
	bind $parent.root <Motion> "%W postcascade @%y"		
	$parent configure -menu $parent.root
}

proc GenerateTreeVariableMenus {member item parent command} {
# Recursive procedure to create menus
	global treeVariableName variable
	set wymax [winfo vrootheight .]
	menu $parent.$item -tearoff 0
	set scrollMenu 0
	foreach entry $treeVariableName($member) {
# Does this entry have a submenu?
		if {[info exist treeVariableName($member.$entry)] == 0} {
# No. Set it as a regular menu item
# If the menu has become too tall to fit on the root window we make a scrollable menu
			if {[$parent.$item yposition last] > $wymax-100 && $scrollMenu == 0} {
				$parent.$item insert 0 command -image uparrow -activebackground green
				$parent.$item add command -image downarrow -activebackground green
				set scrollMenu 1
			}
			if {$scrollMenu == 0} {
				$parent.$item add command -label $entry -command "$command $member.$entry" \
				-activebackground yellow
			}
		} else {
# Yes. Recursively create the submenu and cascade it to the item
			GenerateTreeVariableMenus $member.$entry $entry $parent.$item $command
			if {$scrollMenu == 0} {
				$parent.$item add cascade -label $entry -menu $parent.$item.$entry \
				-background lightyellow -activebackground yellow
			}
		}
	}
# Bindings for scrollable menu
	if {$scrollMenu} {
		bind $parent.$item <Enter> "ScrollVariableMenu $parent.$item $member {$command}"
		bind $parent.$item <ButtonRelease-1> CancelScrollMenu
		bind $parent.$item <Leave> CancelScrollMenu
	}
# Allows to cascade submenus without having to click on the item
	bind $parent.$item <Motion> "%W postcascade @%y"
}	

proc ScrollVariableMenu {wmenu member command} {
	global treeVariableName afterScrollMenu
	set afterScrollMenu [after 20 "ScrollVariableMenu $wmenu $member {$command}"]
	set activeItem [$wmenu index active]
	if {$activeItem != 0 && $activeItem != [$wmenu index last]} {
		update
		return
	}

# Make a list of our entries and find indexes of first and last
	set i 0
	set lastItem [expr [$wmenu index last]-1]
	foreach entry $treeVariableName($member) {
		if {[string equal [$wmenu entrycget 1 -label] $entry]} {set indexFirst $i}
		if {[string equal [$wmenu entrycget $lastItem -label] $entry]} {set indexLast $i}
		lappend lentries $entry
		incr i
	}

# Process Down arrow
	if {$activeItem == [$wmenu index last]} {
# if the last menu item is on the last entry we do nothing and return
		if {$indexLast+1 == [llength $lentries]} {return}
# For the main body of the menu, shift all items up
		for {set i 1} {$i < $lastItem} {incr i} {
# If the types of entries are different we need to delete the old entry and insert a new with the right type
			set newType [$wmenu type [expr $i+1]]
			if {![string equal [$wmenu type $i] $newType]} {
				$wmenu delete $i
				$wmenu insert $i $newType
			}
			$wmenu entryconfigure $i -label [$wmenu entrycget [expr $i+1] -label]
			$wmenu entryconfigure $i -background [$wmenu entrycget [expr $i+1] -background]
			$wmenu entryconfigure $i -activebackground [$wmenu entrycget [expr $i+1] -activebackground]
			if {[string equal $newType cascade]} {
				$wmenu entryconfigure $i -menu [$wmenu entrycget [expr $i+1] -menu]
			} else {
				$wmenu entryconfigure $i -command [$wmenu entrycget [expr $i+1] -command]
			}
		}
# The last item gets the new entry
		set newEntry [lindex $lentries [expr $indexLast+1]]
# Does this entry have a submenu?
		if {[info exist treeVariableName($member.$newEntry)] == 0} {
			set newType command
		} else {
			set newType cascade
		}
# If the types of entries are different we need to delete the old entry and insert a new with the right type
		if {![string equal [$wmenu type $lastItem] $newType]} {
			$wmenu delete $lastItem
			$wmenu insert $lastItem $newType
		}
		if {[string equal $newType command]} {
			$wmenu entryconfigure $lastItem -label $newEntry -command "$command $member.$newEntry" \
			-activebackground yellow
		} else {
			$wmenu entryconfigure $lastItem -label $newEntry -menu $wmenu.$newEntry \
			-background lightyellow -activebackground yellow
		}
	}

# Process Up arrow
	if {$activeItem == 0} {
# if the first menu item is on the first entry we do nothing and return
		if {$indexFirst == 0} {return}
# For the main body of the menu, shift all items down
		for {set i $lastItem} {$i > 1} {incr i -1} {
# If the types of entries are different we need to delete the old entry and insert a new with the right type
			set newType [$wmenu type [expr $i-1]]
			if {![string equal [$wmenu type $i] $newType]} {
				$wmenu delete $i
				$wmenu insert $i $newType
			}
			$wmenu entryconfigure $i -label [$wmenu entrycget [expr $i-1] -label]
			$wmenu entryconfigure $i -background [$wmenu entrycget [expr $i-1] -background]
			$wmenu entryconfigure $i -activebackground [$wmenu entrycget [expr $i-1] -activebackground]
			if {[string equal $newType cascade]} {
				$wmenu entryconfigure $i -menu [$wmenu entrycget [expr $i-1] -menu]
			} else {
				$wmenu entryconfigure $i -command [$wmenu entrycget [expr $i-1] -command]
			}
		}
# The first item gets the new entry
		set newEntry [lindex $lentries [expr $indexFirst-1]]
# Does this entry have a submenu?
		if {[info exist treeVariableName($member.$newEntry)] == 0} {
			set newType command
		} else {
			set newType cascade
		}
# If the types of entries are different we need to delete the old entry and insert a new with the right type
		if {![string equal [$wmenu type 1] $newType]} {
			$wmenu delete 1
			$wmenu insert 1 $newType
		}
		if {[string equal $newType command]} {
			$wmenu entryconfigure 1 -label $newEntry -command "$command $member.$newEntry" \
			-activebackground yellow
		} else {
			$wmenu entryconfigure 1 -label $newEntry -menu $wmenu.$newEntry \
			-background lightyellow -activebackground yellow
		}
	}
}

