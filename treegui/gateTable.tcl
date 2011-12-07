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

package provide gateTable 1.0

##
#  provides a table that lists gate information.
#  This is just a wrapped ttk::treeview and scrollbar pair.
#  The wrapping provides additional and methods:
#
#  OPTIONS:
#   -gates         - list of gate definitions to display.
#   -sortfield     - One of name, type, definition specifying which field is sorted on.
#   -sortdirection - One of ascending/descending specifying the sort order.
#   -command       - Specifies a script to run when an item is double clicked.
#
# METHODS:
#   getsel         - Get the list of gate names that are now selected.
#
# BINDINGS:
#   -  Clicking on a column selects that column as the sorted column.
#      in ascending order.  If the column is already selected the sort order is reversed.
#   -  B1-Motion drags the selection endpoint.  The other selection operations <Control-B1>
#      <Shift-B1> are also used.
#   -  Double-clicking an item fires the -command where %N can be substituted for
#      with the name of the gate that was clicked under the cursor.
#
# @note gates descriptions are lists which ahve the following element in order
#  - Name of the gate.
#  - Unique identifier of the gate.
#  - Gate type code.
#  - Gate definition string (usually also a list).
# 
snit::widget gateTable {
    option -gates         -default [list] -configuremethod SetGates
    option -sortfield     name
    option -sortdirection ascending
    option -command       [list]

    delegate option * to tree

    # Several options can schedule a repopulation of the tree.
    # Rather than do all of those operations, updates are scheduled
    # and this variable is non-zero if an upate is already scheduled.
    # zero if it is

    variable updatePending 0;	# True if an update has been scheduled.

    #  This variable is an array that maps field names to gate indices:

    typevariable fieldMap -array {
	name 0
	type 2
	definition 3
    }
    typevariable sortMap -array {
	ascending -increasing
	descending -decreasing
    }

    


    ##
    # Build the widget, and establish the bindings that lead to 
    # callbacks and internally autonomous behavior.
    #
    # @param args - option name value pairs that configure the widget.

    constructor args {
	
	# Build the widgets.
	set headings [list Name Type Definition]
	install tree using ttk::treeview $win.t -columns $headings -selectmode extended -show headings \
	    -yscrollcommand [list $win.s set]
	foreach column $headings {
	    $win.t heading $column -text $column -command [mymethod changeSort $column] -anchor w
	}
	ttk::scrollbar $win.s -command [list $win.t yview]
xc


	# Lay them out

	grid $win.t $win.s -sticky nsew

	# Process the options last.  This will schedule an update to populate the
	# tree if necessary.  If not necesary there's nothing to display so no need
	# to force an update.

	$self configurelist $args

    }
    #---------------------------------------------------------------------------------
    # Event handling for internal callbacks.
    #

    ##
    # Change the sort field/order and schedule an update.
    # This method is called in response to a click on a column header.
    # @param header - Name of the header that was clicked.
    #
    method changeSort header {
	
	# Lower case the column header to get the field name:

	set field [string tolower $header]

	# If the sort field changed we go to ascending on that field, otherwise just flip the order.
	# The new options(-sortfield) options(-sortdirection) get updated.

	if {$field != $options(-sortfield)} {
	    set options(-sortdirection) ascending
	} else {
	    
	    # Otherwise just flip the sort orderl

	    if {$options(-sortdirection) eq "ascending"} {
		set options(-sortdirection) "descending"
	    } else {
		set options(-sortdirection) "ascending"
	    }
	}
	set options(-sortfield) $field

	# Mark the sort column with the appropriate indicator.
	
	# $self MarkSortColumn

	# schedule a tree update.

	$self ScheduleUpdate
    }

    #---------------------------------------------------------------------------------
    # Option management methods:

    ##
    #  Called when -gates is reconfigured.
    #  updates options(-gates) and schedules an update of the tree.
    # @param option - option name which pretty much shouild be -gates.
    # @param value  - List of gate definitions.
    #
    method SetGates {option value} {
	set options($option) $value

	$self ScheduleUpdate
    }

    #---------------------------------------------------------------------------------
    # Internal methods

    ##
    # ScheduleUpdate
    #   Schedules a repaint of the gates in the widget...this is done via an after 2
    #   but only if $udpatePending is not true.
    #
    method ScheduleUpdate {} {
	if {!$updatePending} {
	    set updatePending 1
	    after 2 [mymethod Update]
	}
    }

    ##
    # Perform an update of the widget
    # - figure out the order of the gates (using the sorting column and direction
    # - clear the tree of all items.
    # - add the gate elements one by one to the three.
    # - reset the updatePending flag
    #
    method Update {} {
	set gates [$self SortGates]
	$win.t delete [$win.t children {} ]

	foreach gate $gates {
	    set name [lindex $gate 0]
	    set type [lindex $gate 2]
	    set def  [lindex $gate 3]

	    $win.t insert {} end -values [list $name $type $def]
	}

	set updatePending 0
    }
    ##
    # return the -gates sorted as per the -sortfield and -sortdirection
    # options.
    #
    # @return
    # @retval list of gates sorted appropriately.
    #
    method SortGates {} {
	# Map the sort field to indices:

	set sortIndex $fieldMap($options(-sortfield))
	set sortDirection $sortMap($options(-sortdirection))
	return [lsort $sortDirection -index $sortIndex -ascii $options(-gates)]
    }
}   