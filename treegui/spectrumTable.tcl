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
package require treeUtilities

package provide spectrumTable 1.0

#
#  The following code is needed to establish our location in the file system
#  specifically so that we can, in turn locate the sorting arrow widgets.
#  This cannot be done in the constructor as there [info script] returns the
#  script that is invoking the constructor and that could be elsewhere
#  (trust me I've tried that and been burned).

namespace eval ::spectrumTable {
    set dirname [file dirname [info script]]
}

##
# Provides a table that lists Spectrum information.
# for each spectrum there are columns for name, type, x parameter name, X parameter low, high limits
# and binning and same information for a Y parameter. Finally there is a column for the applied gate.
#
# OPTIONS:
#  -spectra - supplies/gets the set of spectrum definitions that are being displayed.
#             This is a Tcl list of one element per spectrum.  The elements are themselves
#             lists with elements  that contain in order
#             spectrum-name, type, X parameter, Xlow, Xhi, Xbins, Yparameter, YLow, Yhigh, Bins, 
#             Applied Gate.
# -selectcmd - Script that is invoked when a spectrum is double-clicked. %W can be translated
#              to the widget name, %N the name of the spectrum clicked on.
# METHODS:
#   getSelection - gets a list of the names of spectra that are in the selection.
#
# BINDINGS:
#   - clicking on a column selects the column to be sorted in ascendnig order.  If the column
#     is already selected, the sort order is reversed.
#   - B1-Motion drags the selection endpoint.  The other selection operations <Control-B1>
#     <Shift-B1> are implemented by the underlying tree widget as it is run with 
#     -selectmode extended
#   - Double clicking an item fires the -selectcmd callback if defined.
#

snit::widget spectrumTable {
    hulltype ttk::frame

    option -spectra -default [list] -configuremethod SetSpectra
    option -selectcmd -default [list]

    #
    # Let the user configure the tree:
    #

    delegate option * to tree
    delegate method * to tree

    #
    # Several things can schedule an update.  Rather than do all operations, updates are scheduled
    # and any updates that might happen while the scheduled update is pending don't cause any additional
    # updates.  This hopefully reduces the set of updates required.
    #

    variable updatePending 0;	# non zero if an update is scheduled.

    typevariable updateScheduleTime

    # Sort icon images. 

    typevariable uparrow
    typevariable downarrow
    typevariable sortDirectionImage
    typevariable resampleFactor

    # Map the sort direction to an arrow:


    # Column information:

    typevariable  headings 
    typevariable  titles
    typevariable  widths   


    # Variables used to keep track of mouse motion for stretching/shrinking the selection.

    variable  lastX
    variable  lastY
    variable  lastItem [list]

    variable sortColumn Name
    variable sortDirection -increasing


    #------------------------------------------------------------------------------
    #  Constructors:

    ##
    # Initialize the type variables.
    #
    typeconstructor {

	set updateScheduleTime 2
	set resampleFactor 40

	set dirname $spectrumTable::dirname

	set img [image create photo \
		     -file [file join $dirname uparrow.gif]]
	set uparrow   [image create photo]
	$uparrow copy $img -subsample $resampleFactor $resampleFactor
	image  delete $img

	set img [image create photo \
		     -file [file join $dirname downarrow.gif]]
	set downarrow [image create photo]
	$downarrow copy $img -subsample $resampleFactor $resampleFactor
	image delete $img

	array set sortDirectionImage [list -increasing $uparrow -decreasing $downarrow]


	set headings [list Name Type Xname Xlow Xhigh Xbins Yname Ylow Yhigh Ybins Gate]
	set titles   [list Name Type "X Parameter" Low High Bins "Y Parameter" Low High Bins Gate]
	set widths   [list 175  35   100  30 30 30 100 30 30 30 75]

    }

    ##
    # Construct the widget, lay it out and attache event/bindings handlers.
    # @args - option/values that configure the intial state of the megawiget.
    #
    constructor args {
	#  In this case the headings names differ somewhat from their titles so that 
	#  the headings names can be unique.
	#
	
	# Install the treeview (tree) component  and configure the underlying tree:

	install tree using ttk::treeview $win.t -columns $headings -selectmode extended -show headings \
	    -yscrollcommand [list $win.s set]


	foreach column $headings title $titles width $widths {
	    $tree heading $column -text $title -command [mymethod ChangeSort $column] -anchor w
	    $tree column $column -width $width -anchor w -stretch 1
	}
	$tree heading Name -image $uparrow; # Initial sort is ascending on the name column.

	# Add the scrollbar in case the number of spectra make it worthwhile:

	scrollbar $win.s -command [list $win.t yview]
	

	# Now we can confgure and make visible:

	$self configurelist $args
	grid $tree $win.s -sticky nsew
	grid columnconfigure $win 0 -weight 1
	grid columnconfigure $win 1 -weight 0; # Should keep the scroll bar from scaling.

	# Event bindings:

	bind $tree <Double-1>      [mymethod OnDoubleClick %x %y]
	bind $tree <ButtonPress-1> [mymethod StartDrag %x %y]
	bind $tree <B1-Motion>     [mymethod DragTo %x %y]

    }
    #------------------------------------------------------------------
    #
    # Public methods:
    
    ##
    # Return the names of the selected spectra.  
    # @return list
    # @retval Possibly empty list consisting of the names of the selected spectra.
    #
    method getSelection {} {
	set result [list]
	set items [$tree selection]
	foreach item $items {
	    lappend result [lindex [$tree item $item -values] 0]
	}
	return $result
    }
    #------------------------------------------------------------------
    # Event and bindings handlers.  

    ## 
    # Handle double clicks on a specific element.  Identifies the name of
    # the spectrum and passes control to the -selectcmd callback
    # @param x,y  - widget relative coordinates of the event.
    #
    method OnDoubleClick {x y} {
	set item [$tree identify row $x $y]
	if {$item ne ""} {
	    set data [$tree item $item -values]
	    set name [lindex $data 0]
	    


	    ::treeutility::dispatch $options(-selectcmd) [list %W %N] [list $win $name]
	}
    }
    ##
    # Invoked when a column header is clicked. This changes the sort field and potentially 
    # the order if the sort field was what was changed.
    # @param header - identifies the header that was clicked (this is the column name not
    #                 the title.
    #
    method ChangeSort header {
	
	# If the sort field changed we sort ascending on that field... Otherwise
	# Just flip the order on the current field.

	if {$header ne $sortColumn} {
	    set sortColumn $header
	    set sortDirection -increasing
	} else {
	    if {$sortDirection eq "-increasing"} {
		set sortDirection -decreasing
	    } else {
		set sortDirection -increasing
	    }
	}
	# Set the sort icon and schedule the update which actually sorts the table.

	$self MarkSortColumn $header $sortDirection

	$self ScheduleUpdate
    }
    ##
    # Button 1 press handler.  This establishes the starting point of a drag.
    # The starting point is loaded into the lastX, lastY and the id of the item
    # under that pointer is loaded into lastItem
    #
    # @param x,y - Widget relative coordinates of the click. This is used to
    #              determine the item the pointer was on.
    #
    method StartDrag {x y} {
	set lastX $x
	set lastY $y
	set lastItem [$tree identify row $x $y]
    }
    ##
    # Invoked as a result of a button 1 drag.
    # How the selection changes depends on a bunch-o-stuff.
    # Changes in general happen when we enter a new item.
    # - If we are entering a selected item and the item
    #   away from the direction of motion after the left item
    #   was not selected, we unselect the last item.
    # - Otherwise we add the entered item to the selection.
    # 
    # Regardless, lastX, lastY, lastItem are updated.
    #
    # @param x,y - widget relative position of the pointer.
    #
    method DragTo {x y} {
	set item [$tree identify row $x $y]
	if {$item ne $lastItem} {

	    # figure out which direction we're moving.

	    set dy [expr $y - $lastY]
	    if {$dy < 0} {
		set direction up
	    } else {
		set direction down
	    }
	    # if the new item is not in the selection
	    # it should be added:

	    set selection [$tree selection]
	    if {$item ni $selection} {
		$tree selection add $item
	    } else {
		# Figure out if lastItem needs to be
		# deselected

		if {$direction eq "up"} {
		    set op next
		} else {
		    set op prev
		}
		set awayItem [$tree $op $lastItem]
		if {$awayItem ni $selection} {
		    $tree selection remove $lastItem
		}
	    }
	}
	set lastX $x
	set lastY $y
	set lastItem $item
    }
    #-----------------------------------------------------------------------
    # Option management.


    ##
    # Called to provide a new list of spectra to display.
    # @param option - name of the option being configured (-spectra).
    # @param value  - List of spectra.  Each spectrum is an 11 element list
    #                 containing in order:
    #              - The spectrum name.
    #              - The SpecTcl Spetrum type.
    #              - The X Parameter name.
    #              - The low limit of the X axis.
    #              - The high limit of the X axis.
    #              - The number of bins on the X axis.
    #              - The name of the Y parameter (empty if there isn't one).
    #              - The low limit of the Y axis (empty if not applicable).
    #              - The high limit of the Y axis (empty if not applicable)
    #              - The number of bins on the Y axis (empty if not applicable).
    #              - The applied gate (empty if not applicable).
    # 
    method SetSpectra {option value} {
	set options($option) $value
	
	$self ScheduleUpdate
    }

    #----------------------------------------------------------------------
    #
    # Internal (private) methods.

    ##
    # Schedule an update of the table.  This will actually occur after
    # updateScheduleTime to allow other updates to happen without triggering
    # a pile of redraws.
    #
    method ScheduleUpdate {} {
	if {!$updatePending} {
	    set updatePending 1
	    after $updateScheduleTime [mymethod Update]
	}
    }
    ##
    # Do the actual update of the contents of the widget.
    # - Clear the tree.
    # - Sort the spectra in accordance with the requirements of sortColumn and sortDirection.
    # - Add  the spectra to the tree.
    # - Reset the updatePending flag.
    #
    method Update {} {
	$tree delete [$tree children {} ]

	set spectra [$self SortSpectra]

	foreach spectrum $spectra {
	    $tree insert {} end -values $spectrum
	}

	set updatePending 0
    }
    ##
    # Return a sorted list of spectra.  The sortColumn and sortDirection
    # are used to do the sort.
    # @return list of lists.
    # @retval each list element is an 11 element list as described by SetSpectra.
    #
    method SortSpectra {} {
	set listIndex [lsearch $headings $sortColumn]

	return [lsort $sortDirection -dictionary -index $listIndex $options(-spectra)]
    }
    ##
    # Mark the sort column with the correct sorting glyph.
    # Any glyphs on other columns are removed.
    # @param field - The field being sorted on
    # @param direction - The sort direction.
    #
    method MarkSortColumn {field direction}  {

	set glyph $sortDirectionImage($direction)

	foreach column $headings {
	    if {$column eq $field} {
		$tree heading $column -image $glyph
	    } else {
		$tree heading $column -image [list]; # not the sort column no image.
	    }
	}
    }


    
}