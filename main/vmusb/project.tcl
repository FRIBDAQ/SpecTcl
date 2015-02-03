#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321



#  Private procs will be inside a namespace to prevent 
#  pollution of the global namespace.
#

namespace eval project2D {
    variable topLevelIndex 0;		# In case more than one is active.
    variable direction     x;		# Unfortunately common between all boxes.
    variable inAOI         0;		# True if projection is within an aoi.

}
#
#  Return a list of the contours that are visible on this
#  specturm.
proc project2D::relevantContours spectrum {
    set gates [gate -list]
    set relevant [list]
    set specinfo [lindex [spectrum -list $spectrum] 0]
    set specparams [lindex $specinfo 3]

    foreach gate $gates {
	set type [lindex $gate 2]
	if {$type eq "c"} {
	    set info   [lindex $gate 3]
	    set params [lindex $info 0]
	    set xparam [lindex $params 0]
	    set yparam [lindex $params 1]
	    set name   [lindex $gate 0]
	    if {([lsearch $specparams $xparam] != -1)   &&
		([lsearch $specparams $yparam] != -1)} {
		lappend relevant $name
	    }
	}
    }
    return $relevant
}
#
#  Copy the seleted gate from the listbox to the label:
#
proc project2D::setAoi {list label} {
    $label config -text [$list get active]
}

#  If the AOI is turned off, ghost the listbox and 
#  clear the label:
#
proc project2D::toggleAOI {list label} {
    if {!$project2D::inAOI} {
	$list config -state disabled
	$label config -text {}
	bind $list <Double-1> [list]
	bind $label <Double-1> [list]
    } else {
	$list config -state normal
	bind $list  <Double-1> [list project2D::setAoi $list $label]
	bind $label <Double-1> [list $label config -text {}]
    }
}

# Process the ok click for the projection dialog.
# We need to get the spectrum name, it must be blank and,
# if a duplicate we must warn about that.
# we need to know if there's an aoi.. if so add it to the
# project command that we execute.
# So the external inputs are the data in the project2D namespace 
# and 
#   $top.basespec     - label containing the 2d spectrum name.
#   $top.newname      - label containing name of new spectrum
#   $top.selectedaoi  - label containing the selected aoi.  
#
proc project2D::ok top {
    set name     [$top.newname get]
    set basename [$top.basespec cget -text]

    if {$name eq ""} {
	tk_messageBox -title {Need name} -icon error \
	    -message "You need to give the projection spectrum a name"
	return
    }
    # 
    # Confirm ovewrite on duplicate:
    #
    if {[llength [spectrum -list $name]] ne 0} {
	set answer [tk_messageBox -title {Replace?} -type yesno \
			-icon warning                           \
			-message "$name already exists replace?"]
	if {$answer eq "no"} {
	    return
	} else {
	    spectrum -delete $name
	}
    }
    # now we have a clear shot to making the projection start constructing
    # the command:

    set command "project $basename $name $project2D::direction"

    # See if this is within a gate:

    if {$project2D::inAOI} {
	set gate [$top.selectedaoi cget -text]
	if {$gate ne ""} {
	    append command " $gate"
	}
    }
    # Attempt the projection:

    if {[catch {eval $command} msg]} {
	tk_messageBox -message "Project command '$command' failed: $msg" \
	    -icon error -title {Project failed}
    } else {
	catch {sbind $name};		# Not critical if it fails.
	destroy $top
    }


}

#
#
#   This file contains a little GUI for defining projection spectra.
#   we may want to later add this to the SpecTcl standard stuff if we
#   like it
#
#
#  Gui layout:
#      +------------------------------------------+
#      | Describe projection for <spectrumname>   |
#      |  ( ) Project X    ( )  Project Y         |
#      |                                          |
#      |  +------------------------+              |
#      |  |   list of gates (AOI)  | [ ] in AOI   |
#      |  |                        |              |
#      |  +------------------------+              |
#      |  <name of gate selected if aoi checked>  |
#      |  [ New spectrum name ]                   |
#      +------------------------------------------+
#      |  [ OK  ]   [ Cancel ]                    |
#      +------------------------------------------+
#
#  Note the AOI selection part is only present if there is at least
#  one contour gate visible on that spectrum.
#
# Layout the GUI and establish the handlers... all handlers will get
# the base widget name so they are sensitive to layout changes.
#
proc Project2D baseName {
    set top [toplevel .project_$project2D::topLevelIndex]
    incr project2D::topLevelIndex
    wm title $top {Project spectrum $baseName}

    # Set up The GUI:

    label $top.title      -text {Describe projection for }
    label $top.basespec   -text $baseName

    radiobutton $top.x                 \
	-text {Project X}              \
	-variable project2D::direction \
	-value x
    radiobutton $top.y                 \
	-text {Project Y}              \
	-variable project2D::direction \
	-value y
    set project2D::direction x;		# Set initial radio value to x.

    # Grid what we have as it's not conditional:

    grid $top.title   $top.basespec
    grid $top.x       $top.y

    #  Enumerate the gates first to see if we need to display the AOI stuff.
    #  If not..well bag it.
    #  This will get laid out as well if applicable.

    set contours [project2D::relevantContours $baseName]
    if {[llength $contours] != 0} { 
	checkbutton $top.aoi -text {in AOI} -variable project2D::inAOI \
	    -command [list project2D::toggleAOI $top.which $top.selectedaoi]
	listbox     $top.which -yscrollcommand [list $top.aoisb set] 
	scrollbar   $top.aoisb -command [list $top.which yview] -orient vertical
	foreach contour $contours {
	    $top.which insert end $contour
	}


	label      $top.aoilbl       -text AOI: 
	label      $top.selectedaoi  -width 32

	grid $top.aoi         x
	grid $top.which     -row 3 -column 0 -sticky ewns
	grid $top.aoisb     -row 3 -column 1 -sticky wns
	grid $top.aoilbl    $top.selectedaoi    
	
	# double clicks on the list box set the value of the selected AOI.
	# double clicks on the label blank it out.

	if {!$project2D::inAOI} {
	    $top.which config -state disabled
	} else {	
	    bind $top.selectedaoi <Double-1> [list $top.selectedaoi config -text {}]
	    bind $top.which       <Double-1> \
		[list project2D::setAoi $top.which $top.selectedaoi]
	}


    } else {
	set project2D::inAOI 0;		# by definition not in an aoi.
    }
    #  Now an entry for the new spectrum name:
    #
    label $top.newlabel -text {Name of new spectrum:}
    entry $top.newname -width 32
    grid $top.newlabel
    grid $top.newname   -

    
    # Finally the action area.. in a frame:

    frame $top.action         -relief groove -borderwidth 3
    button $top.action.ok     -text Ok       -command [list project2D::ok $top]
    button $top.action.cancel -text Cancel   -command [list destroy $top]
    grid $top.action.ok     -row 0 -column 0 -sticky w
    grid $top.action.cancel -row 0 -column 1 -sticky e
    grid $top.action        - -sticky ew
}