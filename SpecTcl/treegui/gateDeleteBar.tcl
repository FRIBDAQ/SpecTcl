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

package provide gateDeleteBar

##
# Provides a megawidget that contains the 
# Delete selected and delet all buttons.
# 
# OPTIONS
#   -deleteselected - script to run on delete selected.
#   -deleteall      - script to run on delete all button.
#
# 
snit::widget gateDeleteBar {
    option -deleteselected -default [list]
    option -deleteall      -default [list]

    ##
    # Construct the widget.
    #
    # @param args - options (note there are no legal
    #               options 
    #
    constructor args {
	$self configurelist $args

	ttk::button $win.delsel -text "Delete Selected" \
	    -command [mymethod Dispatch -deleteselected]

	ttk::button $win.delall -text "Delete All" \
	    -command [mymethod Dispatch -deleteall]

	grid $win.delsel $win.delall -sticky ew
    }

    ##
    # Dispatch a callback from an option value
    #
    # @param option - the option that contains the script.
    #
    method Dispatch option {
	set script $options($option)
	if {$script ne ""} {
	    uplevel #0 $script;	# There are no substs.
	}
    }

}