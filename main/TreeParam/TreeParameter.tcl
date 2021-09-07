# (C) Copyright Michigan State University 2019, All rights reserved 
# TreeParameter.tcl
# Author: D. Bazin
# Date: July 2001 - Modified September 2002
# Version 1.2 November 2003

proc UpdateTreeParameter {parameter} {
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set p [lindex [treeparameter -list $parameter] 0]
	set theName [lindex $p 0]
	set thePath [split $theName .]
	set theLength [llength $thePath]
	set Name ""
	for {set j 0} {$j < [expr $theLength-1]} {incr j} {
		append Name [lindex $thePath $j] .
	}
	set Name [string trimright $Name .]
	set theIndex [lsearch -exact $treeParameterName($Name) [lindex $thePath end]]
	set treeParameterBins($Name) [lreplace $treeParameterBins($Name) $theIndex $theIndex [lindex $p 1]]
	set treeParameterStart($Name) [lreplace $treeParameterStart($Name) $theIndex $theIndex [lindex $p 2]]
	set treeParameterStop($Name) [lreplace $treeParameterStop($Name) $theIndex $theIndex [lindex $p 3]]
	set treeParameterInc($Name) [lreplace $treeParameterInc($Name) $theIndex $theIndex [lindex $p 4]]
	set treeParameterUnit($Name) [lreplace $treeParameterUnit($Name) $theIndex $theIndex [lindex $p 5]]
}

proc UpdateTreeParameters {} {
# This procedure stuffs the various array variables from the parameter names
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set theList [treeparameter -list]
	foreach e $theList {
		set theName [lindex $e 0]
#		set theId [lindex $e 1] (was used with the command parameter -list)
		set theBins [lindex $e 1]
		set theStart [lindex $e 2]
		set theStop [lindex $e 3]
		set theInc [lindex $e 4]
		set theUnit [lindex $e 5]
		set thePath [split $theName .]
		set theLength [llength $thePath]
		set rootName [lindex $thePath 0]
# Add parameter root name if it hasn't been registered yet
		if {[info exist treeParameterRoot]} {
			if {[lsearch -exact $treeParameterRoot $rootName] == -1} {
			    lappend treeParameterRoot $rootName
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
				if {[set Index [lsearch -exact $treeParameterName($Name) $subName]] == -1} {
					lappend treeParameterName($Name) $subName
					if {$i == [expr $theLength-2]} {
						lappend treeParameterBins($Name) $theBins 
						lappend treeParameterStart($Name) $theStart 
						lappend treeParameterInc($Name)  $theInc 
						lappend treeParameterStop($Name) $theStop
						lappend treeParameterUnit($Name) $theUnit
					} else {
					    lappend treeParameterBins($Name) none 
					    lappend treeParameterStart($Name) none 
					    lappend treeParameterInc($Name) none 
					    lappend treeParameterStop($Name) none 
					    lappend treeParameterUnit($Name) none 
					}
				} else {
					if {$i == [expr $theLength-2]} {
						set treeParameterBins($Name) [lreplace $treeParameterBins($Name) $Index $Index "$theBins "]
						set treeParameterStart($Name) [lreplace $treeParameterStart($Name) $Index $Index "$theStart "]
						set treeParameterStop($Name) [lreplace $treeParameterStop($Name) $Index $Index "$theStop "]
						set treeParameterInc($Name) [lreplace $treeParameterInc($Name) $Index $Index "$theInc "]
						set treeParameterUnit($Name) [lreplace $treeParameterUnit($Name) $Index $Index "$theUnit "]
					}
				}
			} else {
				set treeParameterName($Name) "$subName "
				if {$i == [expr $theLength-2]} {
					set treeParameterBins($Name) "$theBins "
					set treeParameterStart($Name) "$theStart "
					set treeParameterInc($Name) "$theInc "
					set treeParameterStop($Name) "$theStop "
					set treeParameterUnit($Name) "$theUnit "
				} else {
					set treeParameterBins($Name) "none "
					set treeParameterStart($Name) "none "
					set treeParameterInc($Name) "none "
					set treeParameterStop($Name) "none "
					set treeParameterUnit($Name) "none "
				}
			}
		}
	}
}

proc GenerateMenuBitmaps {} {
# Create Up and Down arrows bitmaps
	set upBitmap "#define up_width 9\n#define up_height 5\n
	static unsigned char up_bits\[\] = {\n
	0x10, 0x00, 0x38, 0x00, 0x7c, 0x00, 0xfe, 0x00, 0xff, 0x01};"
	image create bitmap uparrow -data $upBitmap
	set downBitmap "#define down_width 9\n#define down_height 5\n
	static unsigned char down_bits\[\] = {\n
	0xff, 0x01, 0xfe, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x10, 0x00};"
	image create bitmap downarrow -data $downBitmap
}

proc GenerateTreeMenu {parent command} {
# Generate tree variable menu and attach it to the parent widget
	global treeParameterRoot treeParameterName
	destroy $parent.root
	menu $parent.root -tearoff 0
	foreach e $treeParameterRoot {
		GenerateTreeMenus $e $e $parent.root $command
		$parent.root add cascade -label $e -menu $parent.root.$e \
		-background lightyellow -activebackground yellow
	}
	bind $parent.root <Motion> "%W postcascade @%y"		
	$parent configure -menu $parent.root
}

proc GenerateTreeMenus {member item parent command} {
# Recursive procedure to create menus
	global treeParameterName spectrumParameterX spectrumParameterY parameter variable
	set wymax [winfo vrootheight .]
	menu $parent.$item -tearoff 0
	set scrollMenu 0
	foreach entry $treeParameterName($member) {
# Does this entry have a submenu?
		if {[info exist treeParameterName($member.$entry)] == 0} {
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
			GenerateTreeMenus $member.$entry $entry $parent.$item $command
			if {$scrollMenu == 0} {
				$parent.$item add cascade -label $entry -menu $parent.$item.$entry \
				-background lightyellow -activebackground yellow
			}
		}
	}
# Bindings for scrollable menu
	if {$scrollMenu} {
		bind $parent.$item <Enter> "ScrollMenu $parent.$item $member {$command}"
		bind $parent.$item <ButtonRelease-1> CancelScrollMenu
		bind $parent.$item <Leave> CancelScrollMenu
	}
# Allows to cascade submenus without having to click on the item
	bind $parent.$item <Motion> "%W postcascade @%y"
}	

proc CancelScrollMenu {} {
	global afterScrollMenu
	after cancel $afterScrollMenu
}

proc ScrollMenu {wmenu member command} {
	global treeParameterName afterScrollMenu
	set afterScrollMenu [after 20 "ScrollMenu $wmenu $member {$command}"]
	set activeItem [$wmenu index active]
	if {$activeItem != 0 && $activeItem != [$wmenu index last]} {
		update
		return
	}

# Make a list of our entries and find indexes of first and last
	set i 0
	set lastItem [expr [$wmenu index last]-1]
	foreach entry $treeParameterName($member) {
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
		if {[info exist treeParameterName($member.$newEntry)] == 0} {
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
		if {[info exist treeParameterName($member.$newEntry)] == 0} {
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

