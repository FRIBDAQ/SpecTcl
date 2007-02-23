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
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321
#
package provide editGammaDeluxe 1.0
package require Tk
package require snit
package require guiutilities


#  provides an editor for gamma deluxe spectra.
#  The editor has the following (rough) layout:
#
#  +---------------------------------------------------------------+
#  |  +------------------------+      +------------------------+   |
#  |  | browser                |      |    list of xparams     |   |
#  |  |  ...                   | [x->]|                        |   |
#  |  |                        |      +------------------------+   |
#  |  |                        |      xlow     xhigh   xchans      |
#  |  |                        |      +------------------------+   |
#  |  |                        |      |                        |   |
#  |  |                        | [y->]|    list of yparams     |   |
#  |  |                        |      +------------------------+   |
#  |  |                        |      ylow    yhigh    ychans      |
#  |  +------------------------+                                   |
#  +---------------------------------------------------------------+
#
#  Organizationally, the browser is a self sufficient frame and our
#  stuff goes in a fram called myframe.
#

snit::widget editGammaDeluxe {
    option -browser {}

    # The constructor creates the widget and establishes
    # any needed connections between widgets and methods to ensure
    # the GUI functions seemlessly.
    #

    constructor args {
	$self configurelist $args

	set browser $options(-browser)

	set myframe [frame $win.myframe]

	#  The X-> and Y-> buttons move the selected 
	#  parameter(s) into the X/Y axis boxes.

	button $myframe.tox -text {X->} \
	    -command [mymethod selectedToParams $browser x]
       
	button $myframe.toy -text {Y->} \
	    -command [mymethod selectedToParams $browser y]

	#  X list box and its axis specifications:

	listbox $myframe.xparameters -takefocus 0
	scrollbar $myframe.xscroll -orient vertical -takefocus 0

	label $myframe.xlowlabel  -text {Low} 
	label $myframe.xhighlabel -text {High}
	label $myframe.xchanlabel -text {Channels}

	entry $myframe.xlow   -width 5
	entry $myframe.xhigh  -width 5
	entry $myframe.xchans -width 5

	listbox $myframe.yparameters -takefocus 0
	scrollbar $myframe.yscroll -orient vertical -takefocus 0

	label $myframe.ylowlabel  -text {Low}
	label $myframe.yhighlabel -text {High}
	label $myframe.ychanlabel -text {Channels}

	entry $myframe.ylow   -width 5
	entry $myframe.yhigh  -width 5
	entry $myframe.ychans -width 5

	# Now to layout the contents of myframe:

	grid $myframe.tox $myframe.xparameters - - $myframe.xscroll -sticky nsew
	grid x $myframe.xlowlabel $myframe.xhighlabel $myframe.xchanlabel x
	grid x $myframe.xlow $myframe.xhigh $myframe.xchans               x


	grid $myframe.toy $myframe.yparameters - - $myframe.yscroll -sticky nsew
	grid x $myframe.ylowlabel $myframe.yhighlabel $myframe.ychanlabel x
	grid x $myframe.ylow      $myframe.yhigh      $myframe.ychans     x


	# Now layout the entire GUI:]

	pack $browser $myframe -side left -fill y -expand 1

	# Binding:
	#   Double clicks in the listboxes will remove the axis elements from 
	#  the list:

	bind $myframe.xparameters <Double-1> [mymethod removeParameter %W %x %y]
	bind $myframe.yparameters <Double-1> [mymethod removeParameter %W %x %y]
    }
    # setAxis axisname parameter
    #    If the axis parameter is a tree parameter and if
    #    the low/high/chans items for that axis are empty,
    #    Fill them with the tree parameter low/high and default
    #    2-d channel counts:
    # axisname  - Name of axis (x | y).
    # parameter - Name of parameter.
    #
    method setAxis {axisname parameter} {
	set treeinfo [treeparameter -list $parameter]
	if {$treeinfo ne ""} {
	    set treeinfo [lindex $treeinfo 0]
	    set low      [lindex $treeinfo 2]
	    set high     [lindex $treeinfo 3]
	    set Axis     X
	    if {$axisname eq "y"} {
		set Axis Y
	    }
	    set chans $::GuiPrefs::preferences(default${Axis}Channels)
	    if {$chans eq ""} {
		set chans [lindex $treeinfo 1];# No defaults get from treeparam.
	    }

	    set myframe $win.myframe
	    foreach entry {low high chans}  {
		set widget $myframe.${axisname}${entry}
		set current [$widget get]
		if {$current eq ""} {
		    ::setEntry $widget [set $entry]
		}
	    }
	    
	}
    }
    #   selectedToParams browser axis
    # Load the browser selection into a parameter list box.
    #   browser   - The browser widget.
    #   axis      - The axis affected (x/y).
    # Note that we require that the selection be parameters as only they
    # can be loaded:
    #
    method selectedToParams {browser axis} {
	set selected [$browser getSelection]
	set listbox  $win.myframe.${axis}parameters
	
	foreach selection $selected {
	    set name     [pathToName $selected]
	    
	    if {([parameter -list $name] ne "")} {
		$listbox insert end $name
		$self setAxis $axis $name
		
		
	    }
	}
    }
    # removeParameter listbox x y
    #  Respond to a double click in a listbox by removing the item nearest
    #  the cursor from the box:
    # listbox  - The listbox widget.
    # x,y      - mouse hit coordinates.
    #
    method removeParameter {listbox x y} {
	if {[$listbox size] > 0} {
	    $listbox delete @$x,$y
	}

    }
    #  Return the parameters chosen by the user.  This will be a
    #  2 element list of the x and y parameters as sublists.
    #
    method getParameters {} {
	set myframe $win.myframe

	set xParameters [$myframe.xparameters get 0 end]
	set yParameters [$myframe.yparameters get 0 end]


	return [list $xParameters $yParameters]
    }
    #
    #    Return the axis specifications from the entries:
    #
    method getAxes {} {
	set myframe $win.myframe

	set xlow [$myframe.xlow get]
	set xhigh [$myframe.xhigh get]
	set xchans [$myframe.xchans get]

	#  Require the xlow, xhigh be doubles and the xchans an integer:

	if {[string is double -strict $xlow]   &&
	    [string is double -strict $xhigh]  &&
	    [string is integer -strict $xchans]} {
	    set xAxis [list $xlow $xhigh $xchans]
	} else {
	    return [list]
	}

	set ylow   [$myframe.ylow get]
	set yhigh  [$myframe.yhigh get]
	set ychans [$myframe.ychans get]

	if {[string is double -strict $ylow] &&
	    [string is double -strict $yhigh] &&
	    [string is integer -strict $ychans]} {
	    set yAxis [list $ylow $yhigh $ychans]
	} else {
	    return [list]
	}
	return [list $xAxis $yAxis]
    }
    #
    #  Load stuff for an existing spectrum into
    #  the editor listboxes and the axis definition boxes
    #  We assume the editor is completely empty:
    #
    method load {name} {
	set descr   [lindex [spectrum -list $name] 0];   # Get a description of the spectrum.
	set name    [lindex $descr 1]
	set params  [lindex $descr 3]
	set xParams [lindex $params 0]
	set yParams [lindex $params 1]

	set axes    [lindex $descr 4]
	set xaxis   [lindex $axes 0]
	set yaxis   [lindex $axes 1]

	#  Fill the list boxes:

	set myframe $win.myframe

	foreach xparam $xParams {
	    $myframe.xparameters insert end $xparam
	}
	foreach yparam $yParams {
	    $myframe.yparameters insert end $yparam
	}
	#  Fill the axis specifications:

	::setEntry $myframe.xlow [lindex $xaxis 0]
	::setEntry $myframe.xhigh [lindex $xaxis 1]
	::setEntry $myframe.xchans [lindex $xaxis 2]

	::setEntry $myframe.ylow   [lindex $yaxis 0]
	::setEntry $myframe.yhigh  [lindex $yaxis 1]
	::setEntry $myframe.ychans [lindex $yaxis 2]
    }
    #  Reinitialize the editor to empty.
    #
    method reinit {} {
	set myframe $win.myframe
	$myframe.xparameters delete 0 end
	$myframe.yparameters delete 0 end

	$myframe.xlow delete 0 end
	$myframe.xhigh delete 0 end
	$myframe.xchans delete 0 end


	$myframe.ylow delete 0 end
	$myframe.yhigh delete 0 end
	$myframe.ychans delete 0 end


    }
    #
    #  Return the name of the topic that has help for this 
    #  editor.
    #
    method getHelpTopic {} {
	return editGammaDeluxehelp
    }


}
