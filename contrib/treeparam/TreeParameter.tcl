# TreeParameter.tcl
# Author: D. Bazin
# Date: July 2001 - Modified September 2002

proc UpdateTreeParameter {parameter} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	SendMessage "treeparameter -list $parameter"
	set p [lindex [GetResponse] 0]
	set theName [lindex $p 0]
	set thePath [split $theName .]
	set theLength [llength $thePath]
	set Name ""
	for {set j 0} {$j < [expr $theLength-1]} {incr j} {
		append Name [lindex $thePath $j] .
	}
	set Name [string trimright $Name .]
	set theIndex [lsearch $treeParameterName($Name) [lindex $thePath end]]
	set treeParameterBits($Name) [lreplace $treeParameterBits($Name) $theIndex $theIndex [lindex $p 1]]
	set treeParameterStart($Name) [lreplace $treeParameterStart($Name) $theIndex $theIndex [lindex $p 2]]
	set treeParameterStop($Name) [lreplace $treeParameterStop($Name) $theIndex $theIndex [lindex $p 3]]
	set treeParameterInc($Name) [lreplace $treeParameterInc($Name) $theIndex $theIndex [lindex $p 4]]
	set treeParameterUnit($Name) [lreplace $treeParameterUnit($Name) $theIndex $theIndex [lindex $p 5]]
}

proc UpdateTreeParameters {} {
# This procedure stuffs the various array variables from the parameter names
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	SendMessage "treeparameter -list"
	set theList [GetResponse]
	foreach e $theList {
		set theName [lindex $e 0]
#		set theId [lindex $e 1] (was used with the command parameter -list)
		set theBits [lindex $e 1]
		set theStart [lindex $e 2]
		set theStop [lindex $e 3]
		set theInc [lindex $e 4]
		set theUnit [lindex $e 5]
		set thePath [split $theName .]
		set theLength [llength $thePath]
		set rootName [lindex $thePath 0]
# Add parameter root name if it hasn't been registered yet
		if {[info exist treeParameterRoot]} {
			if {[lsearch $treeParameterRoot $rootName] == -1} {
				append treeParameterRoot "$rootName "
			}
		} else {
			set treeParameterRoot "$rootName "
		}
# Fill array variables
		for {set i 0} {$i < [expr $theLength-1]} {incr i} {
			set Name ""
			for {set j 0} {$j <= $i} {incr j} {
				append Name [lindex $thePath $j] .
			}
			set Name [string trimright $Name .]
			set subName [lindex $thePath [expr $i+1]]
			if {[info exist treeParameterName($Name)]} {
				if {[set Index [lsearch $treeParameterName($Name) $subName]] == -1} {
					append treeParameterName($Name) "$subName "
					if {$i == [expr $theLength-2]} {
						append treeParameterBits($Name) "$theBits "
						append treeParameterStart($Name) "$theStart "
						append treeParameterInc($Name) "$theInc "
						append treeParameterStop($Name) "$theStop "
						append treeParameterUnit($Name) "$theUnit "
					} else {
						append treeParameterBits($Name) "none "
						append treeParameterStart($Name) "none "
						append treeParameterInc($Name) "none "
						append treeParameterStop($Name) "none "
						append treeParameterUnit($Name) "none "
					}
				} else {
					if {$i == [expr $theLength-2]} {
						set treeParameterBits($Name) [lreplace $treeParameterBits($Name) $Index $Index "$theBits "]
						set treeParameterStart($Name) [lreplace $treeParameterStart($Name) $Index $Index "$theStart "]
						set treeParameterStop($Name) [lreplace $treeParameterStop($Name) $Index $Index "$theStop "]
						set treeParameterInc($Name) [lreplace $treeParameterInc($Name) $Index $Index "$theInc "]
						set treeParameterUnit($Name) [lreplace $treeParameterUnit($Name) $Index $Index "$theUnit "]
					}
				}
			} else {
				set treeParameterName($Name) "$subName "
				if {$i == [expr $theLength-2]} {
					set treeParameterBits($Name) "$theBits "
					set treeParameterStart($Name) "$theStart "
					set treeParameterInc($Name) "$theInc "
					set treeParameterStop($Name) "$theStop "
					set treeParameterUnit($Name) "$theUnit "
				} else {
					set treeParameterBits($Name) "none "
					set treeParameterStart($Name) "none "
					set treeParameterInc($Name) "none "
					set treeParameterStop($Name) "none "
					set treeParameterUnit($Name) "none "
				}
			}
		}
	}
}

proc GenerateTreeMenu {parent command} {
# Generate tree variable menu and attach it to the parent widget
	global treeParameterRoot treeParameterName
	destroy $parent.root
	menu "$parent.root" -tearoff 0
	foreach e $treeParameterRoot {
		GenerateTreeMenus $e $e "$parent.root" $command
		"$parent.root" add cascade -label $e -menu "$parent.root.$e"
	}
	bind "$parent.root" <Motion> "%W postcascade @%y"		
	$parent configure -menu "$parent.root"
}

proc GenerateTreeMenus {member item parent command} {
# Recursive procedure to create menus
	global treeParameterName
	menu "$parent.$item" -tearoff 0
	foreach entry $treeParameterName($member) {
# Does this entry have a submenu?
		if {[info exist treeParameterName($member.$entry)] == 0} {
# No. Set it as a regular menu item
			set str "$member.$entry"
			"$parent.$item" add command -label $entry -command "$command $str"
		} else {
# Yes. Recursively create the submenu and cascade it to the item
			GenerateTreeMenus "$member.$entry" $entry "$parent.$item" $command
			"$parent.$item" add cascade -label $entry \
			-menu "$parent.$item.$entry"
		}
	}
# Allows to cascade submenus without having to click on the item
	bind "$parent.$item" <Motion> "%W postcascade @%y"
}	

proc GenerateTreePseudoMenu {parent command} {
# Generate tree variable menu and attach it to the parent widget
	global treeParameterRoot treeParameterName
	destroy $parent.root
	menu "$parent.root" -tearoff 0
	foreach e $treeParameterRoot {
		GenerateTreePseudoMenus $e $e "$parent.root" $command
		"$parent.root" add cascade -label $e -menu "$parent.root.$e"
	}
	bind "$parent.root" <Motion> "%W postcascade @%y"		
	$parent configure -menu "$parent.root"
}

proc GenerateTreePseudoMenus {member item parent command} {
# Recursive procedure to create menus
	global treeParameterName
	menu "$parent.$item" -tearoff 0
	foreach entry $treeParameterName($member) {
# Does this entry have a submenu?
		if {[info exist treeParameterName($member.$entry)] == 0} {
# No. Set it as a regular menu item
			set str "$member.$entry"
			"$parent.$item" add command -label $entry -state disabled
		} else {
# Yes. Recursively create the submenu and cascade it to the item
			GenerateTreePseudoMenus "$member.$entry" $entry "$parent.$item" $command
			"$parent.$item" add cascade -label $entry \
			-menu "$parent.$item.$entry"
		}
	}
# Add the only activated entry to allow the definition of a new pseudo parameter
	set str $member
	"$parent.$item" add command -label "Pseudo..." -command "$command $str"
# Allows to cascade submenus without having to click on the item
	bind "$parent.$item" <Motion> "%W postcascade @%y"
}	

