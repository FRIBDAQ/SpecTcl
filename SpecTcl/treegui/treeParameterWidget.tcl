#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2009.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321



package require Tk
package require snit

##
#  Provide a snit megawidget that allows for the display of a single
#  tree parameter.
#  The tree parameter looks like this:
#  +------------------------------------------------------------------------------------+
#  | [parameter name] [low value]  [high value] [units] <Load> <Set> <Change Spectra>   |
#  +------------------------------------------------------------------------------------+
#
#
#  OPTIONS:
#     -name      name of parameter to have in spectrum.
#     -low       get/set low value of parameter in widget.
#     -high      get/set high value of parameter in widget.
#     -units     get/set units of parameter in widget.
#     -loadcmd   Script executed on load button click, supports %W substitution
#     -setcmd    Script executed on  set button click supports %W  substitution.
#     -changecmd Script executed on Change spectra button click supports %W substitution.
#
# METHODS:
#   resetChanged   - Turn off changed flag (this can be done by save).
#
#

snit::widget treeParameterEditor {
    option -name
    option -low
    option -high
    option -units
    option -loadcmd   [list]
    option -setcmd    [list]
    option -changecmd [list]


    ##
    # Constructor 
    #

    constructor args {
	$self configurelist $args

	# Build the widgets:

	# First the labels:

	foreach label [list .name .low .high .unit] optionname [list -name -low -high -units] \
	    width [list 32 5 5 10] {
	    ::ttk::label $win$label -textvariable ${selfns}::options($optionname) -relief sunken \
		-width $width -borderwidth 3 -anchor w
	}

	# then the buttons:

	foreach button [list .load .set .changespectra] \
	    label [list Load Set "Change Spectra"]  \
	    option [list -loadcmd -setcmd -changecmd] width [list 6 6 14] {
		::ttk::button $win$button -text $label -width $width \
		    -command [mymethod callback $option]
	}

	#  Now grid them from left to right:
	
	set col 0
	foreach widget [list .name .low .high .unit .load .set .changespectra] {
	    grid $win$widget -row 0 -column $col
	    incr col
	}
    }
}