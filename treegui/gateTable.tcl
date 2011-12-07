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
    option -sortfield     -default name   -configuremethod SetSortField
    option -sortdirection ascending
    option -command       [list]

    delegate option * to tree
    delegate method * to tree
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

    # Several options can schedule a repopulation of the tree.
    # Rather than do all of those operations, updates are scheduled
    # and this variable is non-zero if an upate is already scheduled.
    # zero if it is

    variable updatePending 0;	# True if an update has been scheduled.
    
    #Images used to label the column sort order

    variable uparrow
    variable downarrow

 
    


    ##
    # Build the widget, and establish the bindings that lead to 
    # callbacks and internally autonomous behavior.
    #
    # @param args - option name value pairs that configure the widget.

    constructor args {
	
	set img [image create photo -file uparrow.gif]
	set uparrow   [image create photo]
	$uparrow copy $img -subsample 40 40
	image  delete $img

	set img [image create photo -file downarrow.gif]
	set downarrow [image create photo]
	$downarrow copy $img -subsample 40 40
	image delete $img


	# Build the widgets.
	set headings [list Name Type Definition]
	install tree using ttk::treeview $win.t -columns $headings -selectmode extended -show headings \
	    -yscrollcommand [list $win.s set]
	foreach column $headings {
	    $win.t heading $column -text $column  -command [mymethod changeSort $column] -anchor w
	}
	ttk::scrollbar $win.s -command [list $win.t yview]

	$win.t heading Name -image $uparrow

	# Lay them out

	grid $win.t $win.s -sticky nsew

	# Process the options last.  This will schedule an update to populate the
	# tree if necessary.  If not necesary there's nothing to display so no need
	# to force an update.

	$self configurelist $args

	# Add the bindings:

	bind $win.t <Double-1> [mymethod onDoubleClick %x %y]
				
    }
    #---------------------------------------------------------------------------------
    # Event handling for internal callbacks.
    
    
    ##
    # Handle double clicks of the mouse.
    # We only need to do something if there is a callback
    # script.  In that case;
    # - figure out the name of the item that was clicked.
    # - If necessary substitute it into the script for 
    #   %N occurences.
    # - invoke the script at level 0.
    # @param x - window relative x position of the click.
    # @param y - window relative y positinoof the click.
    #
    method onDoubleClick {x y} {
	set script $options(-command)

	if {$script ne ""} {
	    set item [$win.t identify row  $x $y]
	    if {$item ne ""} {
		set data [$win.t item $item -values]
		set name [lindex $data 0]

		regsub "%N" $script $name script
		uplevel #0 $script
	    }
	}
    }
    
    
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
	
        $self MarkSortColumn $field $options(-sortdirection)

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
    ##
    # Called to set a new sort field value programmaticall.
    # - options(-sortfield) is updated to match.
    # - The  appropriate column is given a sorting glyph.
    # - A display update is schduled.
    # @param option - name of the configuration option.
    #        this must be -sortfield.
    # @param value - new value of the option
    # 
    method SetSortField {option value} {

	# Be sure this a valid column name:

	if {[lsearch [list name type definition] $value] == -1} {
	    error "Invalid sortcolumn $value"
	}

	# no error so save the option, mark the column
	# and update the widget.

	set options($option) $value

	$self MarkSortColumn $value $options(-sortdirection)

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
    ##
    #
    # - Remove all sorting glyphs from all columns.
    # - Mark the sorting column with the proper glyph.
    # @param col - Colum that is being sorted.
    # @param dir - Sort direction (ascending, descending)
    #
    method MarkSortColumn {col dir} {
	
	# Select the glyph:
	
	if {$dir eq "ascending"} {
	    set image $uparrow
	} else {
	    set image $downarrow
	}
	foreach column [list Name Type Definition] {
	    set c [string tolower $column]
	    if {$c eq $col} {
		$win.t heading $column -image $image
	    } else {
		$win.t heading $column -image [list]
	    }
	}

    }
}   