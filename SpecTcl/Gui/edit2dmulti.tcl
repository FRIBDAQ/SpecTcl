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
#
package provide edit2dmulti 1.0
package require snit
package require browser
package require guiutilities


#  This file contains code to implement the rates GUI.
#

# As far as package requires go, we assume the SpecTcl GUI is up
# and it has imported browser and snit.
# We need this to be true in order to hook ourselves into its gui in any event.

#----------------------------------------------------------------------------
#  editRatesDialog
#
#    This is a snidget to edit a 2-d rates spectrum definition.
#    2-d rates spectra are defined in terms of sets of parameter pairs.
#    parameter pairs can be accepted a parameter at a time from the
#    parameters in the parameter browser or from the browsable set of
#    2-d spectra (each providing an x/y parameter pair).
#    The editor requires that you have the same number of x/y parameters
#    Parameters are paired in the order shown on their respective list boxes.
#
# Widget layout:
#
#   +-------------------------------------------------------------------+
#   | +--------------------+                 () X            () Y       |
#   | | Spec/param browser |          +-------------+    +-----------+  |
#   | |                    |          | X parameter |    |Y parameter|  | 
#   | |                    |          | scrolling   |    | scrolling |  |
#   | |                    |          | listbox     |    | listbox   |  |
#   | |                    |          |             |    |           |  |
#   | |                    |          +-------------+    +-----------+  |
#   | |                    |                   From   To   Bins         |
#   | |                    |          X axis:[    ]  [ ] [    ]         |
#   | |                    |                   From   To   Bins         |
#   | +--------------------+          Y axis:[     ] [ ] [    ]         |
#   +-------------------------------------------------------------------+

#
# Operational details:
#    The browser shows spectrum and parameter folders.  Spectra are
#    restricted to 2ds.
#    double clicking a spectrum will remove add its parmeter pair
#    to the bottom of the X/Y parameter list boxes.
#    double clicking a parameter will add it to the listbox that has the
#    radio button above it selected and flip the radio button selection
#    to the other one.
#      When the first set of parameters are added, if an axis specification
#    is empty, and the parameter has a suggestion for the axis specification,
#    it is loaded into the appropriate axis specification.
#      Double clicking a parametr in the x/y parameter box removes it
#    from the list and its corresponding other parameter.
#    Note that none of these operations remove parameter or spectra from the
#    parameter/spectrum browser because:
#    - It is perfectly reasonable for a paramter to show up more than once.
#    - We don't really know once a spectrum's parameters have been loaded
#      which spectrum they came from.
#
# Options:
#    -browser    - provides a browser widget for ordinary spectra.
# Methods:
#    load          - Load the editor with a spectrum.
#    reinit        - Clear the spectrum definition.
#    getParameters - Returns the parameters that are in each of the listboxes.
#                    parameters are selected... unlike the rest of the
#                    editors, this returns a list of [list x y] parameter
#                    pairs.. if there are more parameters in one box
#                    than the other, The missing parameters from the other
#                    box are substituted with blanks e.g. [list [list] yparam]
#                    It is up to the caller to do any error/detection/handling
#                    on this case.
#    getAxes       - Get the axis specifications for the spectrum.
#    getHelpTopic  - Return the help topic associated with this
#    
#
snit::widget edit2dMulti {
    option -browser

    variable currentParameter x;	# Can be x or y.

    method myVar varname {
	return ${selfns}::$varname
    }

    constructor args {
	$self configurelist $args


	set browser $options(-browser)
	$browser configure                            \
	    -filterspectra   [mymethod only2d]        \
	    -parameterscript [mymethod addParameter]  \
	    -spectrumscript  [mymethod addSpectrum]  
	
	radiobutton $win.xselect \
	    -text X              \
	    -value x             \
	    -variable [$self myVar currentParameter]
	radiobutton $win.yselect \
	    -text Y              \
	    -value y             \
	    -variable [$self myVar currentParameter]


	listbox $win.xparams  -height 8 -yscrollcommand [list $win.xscroll set]
	scrollbar $win.xscroll  -orient vertical -command [list $win.xparams yview]
	listbox $win.yparams  -height 8 -yscrollcommand [list $win.yscroll set]
	scrollbar $win.yscroll  -orient vertical -command [list $win.yparams yview]


	#  The axis definitions:

	frame $win.xaxis
	label $win.xaxis.fromlbl  -text From
	label $win.xaxis.tolbl    -text To
	label $win.xaxis.binslbl  -text Bins
	label $win.xaxis.unitslbl -text Units

	label $win.xaxis.axis  -text {X axis:}
	entry $win.xaxis.from  -width 8
	entry $win.xaxis.to    -width 8
	entry $win.xaxis.bins  -width 10
	label $win.xaxis.units 


	grid     x             $win.xaxis.fromlbl  $win.xaxis.tolbl $win.xaxis.binslbl $win.xaxis.unitslbl
	grid  $win.xaxis.axis  $win.xaxis.from     $win.xaxis.to    $win.xaxis.bins    $win.xaxis.unitslbl

	frame $win.yaxis
	label $win.yaxis.fromlbl  -text From
	label $win.yaxis.tolbl    -text To
	label $win.yaxis.binslbl  -text Bins
	label $win.yaxis.unitslbl  -text Units

	label $win.yaxis.axis  -text {Y axis:}
	entry $win.yaxis.from  -width 8
	entry $win.yaxis.to    -width 8
	entry $win.yaxis.bins  -width 10
	label $win.yaxis.units 
	grid     x             $win.yaxis.fromlbl  $win.yaxis.tolbl $win.yaxis.binslbl $win.yaxis.unitslbl
	grid  $win.yaxis.axis  $win.yaxis.from     $win.yaxis.to    $win.yaxis.bins    $win.yaxis.units


	# Set up the layout of the megawidget.
	
	grid $win.xselect x  $win.yselect x
	grid $win.xparams   -row 1 -column 0
	grid $win.xscroll   -row 1 -column 1 -sticky nsw
	grid $win.yparams   -row 1 -column 2
	grid $win.yscroll   -row 1 -column 3 -sticky nsw
	grid $win.xaxis     -        -       -sticky nsew
	grid $win.yaxis     -        -       -sticky nsew


	# Set actions on the various widgets
	
	#  Double clicks on listboxes will kill of related pairs
	#  of parameters.

	bind $win.xparams <Double-1> [mymethod removeParams $win.xparams  %y]
	bind $win.yparams <Double-1> [mymethod removeParams $win.yparams  %y]

	
    }
    #-----------------------------------------------------------------
    #  The methods below are required by the 
    #  spectrum editor to query this widget.  They are considered
    #  public.  Purists will argue these should be config parameters
    #  but really they are all queries not configurable items.
    #
    
    # reinit
    #  Empties the widgets.

    method reinit {} {
	$win.xparams delete 0 end
	$win.yparams delete 0 end
	
	foreach item [list from to bins] {
	    ::setEntry $win.xaxis.$item ""
	    ::setEntry $win.yaxis.$item ""
	}
        $win.xaxis.units config -text ""
	$win.yaxis.units config -text ""
    }

    # load name
    #   Loads the widgets of the box with a description of the current spectrum.
    #

    method load name {
	$self reinit

	set info [spectrum -list $name]
	set info [lindex $info 0]

	set parameters [lindex $info 3]
	set axes       [lindex  $info 4]

	foreach {x y} $parameters {
	    $win.xparams insert end $x
	    $win.yparams insert end $y
	}

	set xaxis [lindex $axes 0]
	set xlow  [lindex $xaxis 0]
	set xhigh [lindex $xaxis 1]
	set xbins [lindex $xaxis 2]
	set xunits ""

	set xpinfo [treeparameter -list [lindex $parameters 0]]
	if {[llength $xpinfo] != 0} {
	    set xpinfo [lindex $xpinfo 0]
	    set xunits [lindex $xpinfo 5]
	}
	::setEntry $win.xaxis.from $xlow
	::setEntry $win.xaxis.to  $xhigh
	::setEntry $win.xaxis.bins $xbins
	$win.xaxis.units config -text $xunits

	set yaxis [lindex $axes 0]
	set ylow  [lindex $yaxis 0]
	set yhigh [lindex $yaxis 1]
	set ybins [lindex $yaxis 2]
	set yunits ""
	set ypinfo [treeparameter -list [lindex $parameters 1]]
	if {[llength $ypinfo] != 0} {
	    set ypinfo [lindex $ypinfo 0]
	    set yunits [lindex $ypinfo 5]
	}
	::setEntry $win.yaxis.from $ylow
	::setEntry $win.yaxis.to  $yhigh
	::setEntry $win.yaxis.bins $ybins
	$win.yaxis.units config -text $yunits

    }
    # getParameters
    #   Returns the current set of parameters defined.
    # NOTE:  This works a bit differently than ordinary
    #        spectra. x/y parameter pairs are returned, however
    #        some parameter pairs may be incomplete due to extra
    #        parameters in the x or y listboxes.
    #        While this means the caller must special case this code,
    #        it makes error checking possible.. If i just returned parameters
    #        in a single list as 'normal', and the user had 2 extra x parameters
    #        this would not be distinguishable from an additional parameter
    #        pair e.g.
    #
    method getParameters {} {
	set xparams [$win.xparams get 0 end]
	set yparams [$win.yparams get 0 end]

	set result [list]
	foreach x $xparams y $yparams {
	    lappend result [list $x $y]
	}
	return $result
    }

    # getAxes  - Gets the axis specifications for the spectrum.
    #            This is a 2 element list where each subelement is a
    #            standard 3 element low/hi/bins axis specification.
    #            Errors return an empty list.
    #
    method getAxes {} {
	foreach axis [list xaxis yaxis] {
	    set basename $win.$axis
	    set low      [$basename.from get]
	    set high     [$basename.to   get]
	    set bins     [$basename.bins get]

	    # These must be non empty doubles for the limits and int for bins.

	    if {![string is double -strict $low]              ||
		![string is double -strict $high]             ||
		![string is integer -strict $bins]} {
		return [list]
	    }
	    lappend axes [list $low $high $bins]
	}
	return $axes
    }
    #
    #   Returns the name of the help topic for this editor widget.
    #
    method getHelpTopic {} {
	return edit2dmulti
    }
    #------------------------------------------------------------------
    #  The methods and procs below drive the user interface and are
    #  not considered public
    #

    # Add a parameter to the selected list box.
    # The selected list box is indicaed by the value of currentParameter.
    # Once the parameter is added, the currentParameter is advanced so that
    # by default the user can add parameters in x/y pairs.
    #
    # Note that path has Parameter.parametername on it.
    #
    #
    method addParameter path {
	set parameterName [pathToName $path]
	set listbox $win.${currentParameter}params
	$listbox insert end $parameterName
	$self setAxis $currentParameter $parameterName
	$self nextAxis
    }
    # Add a spectrum's parameters to the parameter list.
    #
    # Note that the path has a Spectrum. in it.
    # Note that the spectrum could have been deleted out from underneath us
    # or changed so we validate this as well.
    # putting up a message box, and refreshing the browser if that happened.
    #
    method addSpectrum path {
	set spectrumName [pathToName $path]
	set definition [spectrum -list $spectrumName]
	set definition [lindex $definition 0]
	set type       [lindex $definition 2]
	set parameters [lindex $definition 3]
	set xparam     [lindex $parameters 0]
	set yparam     [lindex $parameters 1]

	# Be sure this is a 2d spectrum... if the spectrum was deleted,
	# type will be blank and also fail the test below:

	if {$type ne "2"} {
	    tk_messageBox                 \
		-icon info                \
		-type ok                  \
		-title "Spectrum not ok"  \
		-message "Spectrum $spectrumName was changed so that it is no longer a 2d or it was deleted"


	    $options(-browser) update
	    return
	}
	#  Add the parameters.
	
	$win.xparams insert end $xparam
	$win.yparams insert end $yparam
	
	# If necessary set the axis from the parameter.

	$self setAxis x $xparam
	$self setAxis y $yparam
    }

    #
    # Remove a pair of related parameters.  Related parameters have the same
    # list index in both list boxes.  If there is no related parameter,
    # then only the entry in the double clicked list box will be removed.
    # This can only happen if the listbox clicked in has more entries than the
    # other one.
    # Parameters:
    #    widget  - the widget in which the double click landed.
    #    y       - the widget y coordinate of the click.
    #
    #
    method removeParams {widget y} {
	set index [$widget nearest $y]
	
	foreach lbox [list $win.xparams $win.yparams] {
	    set size [$lbox index end]
	    if {$index < $size} {
		$lbox delete $index
	    }
	}
    }

    #  Filter only the 2 d spectra.
    #
    method only2d description {
	set stype [lindex $description 2]
	if {$stype eq "2"} {
	    return 1
	}
	return 0
    }

    #  Swap axes:
    #
    method nextAxis {} {
	if {$currentParameter eq "x"} {
	    set currentParameter  "y"
	} else {
	    set currentParameter "x"
	}
    }
    #  Sets the named axis from/to/bins 
    #  according to the parameter if it has this info.
    #
    #  Only sets a field if it is empty... and if the parameter has a definition.
    #
    method setAxis {which parameter} {
	set basename $win.${which}axis
	set paraminfo [treeparameter -list $parameter]
	if {[llength $paraminfo] == 0} {
	    return
	}
	set info [lindex $paraminfo 0]
	set low  [lindex $info 2]
	set high [lindex $info 3]
	set bins [lindex $info 1]
	set units [lindex $info 5]


	if {[$basename.from get ] eq ""} {
	    ::setEntry $win.${which}axis.from $low
	}
	if {[$basename.to get] eq "" } {
	    ::setEntry $win.${which}axis.to   $high
	}
	if {[$basename.bins get] eq ""} {
	    ::setEntry $win.${which}axis.bins $bins
	}
	$basename.units configure -text $units
    }

    #  Strip the first element off a path to create a name in the appropriate
    #  namespace:

    proc pathToName path {
	set pathList [split $path .]
	set pathList [lrange $pathList 1 end]
	set newPath  [join $pathList .]
	return $newPath
    }

}