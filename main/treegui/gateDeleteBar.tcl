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

package provide gateDeleteBar 1.0

##
# Provides a megawidget that contains the 
# Delete selected and delet all buttons.
# 
# OPTIONS
#   -deleteselected - script to run on delete selected.
#   -deleteall      - script to run on delete all button.
#   -metacmd        - Script to trun on metadata.. .button.
# 
snit::widget gateDeleteBar {
    hulltype ttk::frame
    option -deleteselected -default [list]
    option -deleteall      -default [list]
    option -metacmd        -default [list]

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
        ttk::button $win.metadata -text Metadata.. \
            -command [mymethod Dispatch -metacmd]
        grid $win.delsel -sticky w
        grid $win.delall -row 0 -column 1 -sticky e
        grid $win.metadata -row 0 -column 2 -sticky e
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