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

package provide scrollingMenu 1.0


namespace eval scrollingMenu {
    set dirname [file dirname [info script]]; # have to define this while sourceing.
}


##
# Provides a snit widgetadaptor that adapts a menu so that
# it scrolls if the number of entries gets larger than the 
# verticalscreen measure.
#
#  For edges of the menu that are off screen,
#  an arrow graphic is placed at that end (up arrow at the top, down arrow at the bottom).
#  Scrolling is accomplished by entering the scroller arrow with the mouse.  Scrolling continues
#  at a settable rate until the mouse either leaves the scroller arrow or B1 is released (in 
#  which case no menu command fires.
#  
#
snit::widgetadaptor scrollingMenu  {

    # Arrow image handles if not null:

    typevariable uparrow ""
    typevariable downarrow ""


    variable items -array {};	# Initially empty array of menu contents.
    variable itemCount 0;	# Number of menu items.
    variable topIndex  0;	# Item index at the top of the menu (not counting the ^ if present.
    variable bottomIndex 0;	# Item index at the bottom of the menu (not counting the V if present.
    variable scrolling   0; 	#  nonzero if the menu requires scrolling.
    variable timerId     -1;	# Scrolling timer id.

    # Our extended options:

    option -scrolltimer -default 20 -configuremethod timerchanged;	# ms betwee successive scrolls.

    # Unless overridden, all the methods and options are passed through to the underlying
    # menu.

    delegate option * to hull
    delegate method * to hull


    constructor args {
	installhull using menu -postcommand [mymethod _checkEmpty]

	$self configurelist $args
    }
    method timerchanged {opt val} {
        set options($opt) $val
    }
    #-----------------------------------------------------------------------------
    # Public methods:
    #

    ##
    # Intercept the 'add' method so that we can see if it's necessary to 
    # The elements added to the menu are stored in the items array indexed by
    # item position.  Each element consists of a dict with the following values:
    # type - the entry type (e.g. command).
    # options - The set of options that differ from default values.
    # note that some options (e.g. -columnbreak) are not allowed in a scrollingMenu
    # item configuration because they make no sense.
    #
    # @param itemType - the type of menu entry to add.
    # @param args - Option/value pairs that configure then entry:
    #
    method add {itemType args} {

	# If we don't need to scroll just add the entry.
	# otherwise set up scrolling if it's not already set up.
	#


	if {[$hull yposition last] < ([winfo vrootheight .] - 100)} {
	    $hull add $itemType {*}$args
	    set bottomIndex $itemCount;	# last one visible.

	} else {
	    $self StartScrolling

	}
    
	# regardless store the item in the array:
	#

	set items($itemCount) [dict create type $itemType options $args]	
	incr itemCount;	
    }
    #-----------------------------------------------------------------------------
    #
    # Private methods:

    # _checkEmpty
    #
    #   Called in response to a post request. Note that the menu is not yet
    #   posted.
    #   -  If there are no items, pop up a message box waring that's the case.
    #   -  If there are no items, schedule an unpost.
    #  This is addressing:  daqdev/SpecTcl#389
    #
    method _checkEmpty {} {
        if {$itemCount == 0} {
            after 500 $win unpost;              #Schedule the unpost.
            #tk_messageBox -type ok -icon warning -title {Empty menu} \
            #    -message "Pull down menu has no items to choose from"
        }
    }
    
    ##
    # This is called when it's time to setup scrolling for the menu.
    #  The topIndex, bottomIndex items will be displayed but we're going to put in
    #  up/down scrolling arrows as well.  Events get bound to those items to facilitate
    #  the scrolling and the scrolling flag is set true.  
    #  When scrolling gets turned on, top and bottomIndex are already correct.
    #
    method StartScrolling {} {

	# Already scrolling:

	if {$scrolling} {
	    return
	}

	set scrolling 1

	$self LoadImages;	# if necessary, load the up/down arrows.

	# add the image items... these have no commands so they don't do anything..
	# however they do respond to mouse entry to initiate scrolling..

	$hull add command -image $downarrow

	
	bind $win <Enter>           [mymethod ScrollMenu]
	bind $win <ButtonRelease-1> [mymethod CancelScrollingTimer]
	bind $win <Leave>           [mymethod CancelScrollingTimer]


    }

    ##
    #  If images have not yet been loaded, load them and save the image
    #  handles in the uparrow/downarrow typevariables:
    #
    method LoadImages {} {


	if {[string length $uparrow] != 0} {
	    return
	}


	set img [image create photo \
		     -file [file join $scrollingMenu::dirname uparrow.gif]]
	set uparrow [image create photo]
	$uparrow copy $img -subsample 40 40
	image delete $img


	set img [image create photo \
		     -file [file join $scrollingMenu::dirname downarrow.gif]]
	set downarrow [image create photo]
	$downarrow copy $img -subsample 40 40
	image delete $img



    }
    ## 
    # Called in response to an enter event:
    #  - If the scroll timer is not yet set we reschedule ourselves to go again later.
    #  - If we are in the last item and the 
    method ScrollMenu {} {
        set timerId [after $options(-scrolltimer) [mymethod ScrollMenu]]
    
    
        set activeItem [$hull index active]
    
    
        # If the active item is 0 and topIndex != 0 we need to scroll down and insert
        # the prior item into the menu.
        
        if {($activeItem == 0) && ($topIndex != 0)} {
            $self ScrollDown
        }
    
        # If the active item is the last one and the bottomIndex is not last element of the
        # list of menu items, Scroll up:
        
    
        if {($activeItem == [$hull index last]) 
            && ($bottomIndex < ($itemCount - 1)) }  {
            $self ScrollUp
        }
    }
    
    ##
    # Cancel the scrolling timer if it's set:
    #
    method CancelScrollingTimer {} {
        if {$timerId != -1} {
            after cancel $timerId
            set timerId -1
        }
    }
    ##
    # Scroll the menu up
    # - if necessary, add a scroll arrow at the top.
    # - deleting item 1 from the menu.
    # - inserting a new last-1 element in the menu from our array.
    # - if necessary, removing the scroll up arrow
    # - Somewhen adjust the top/bottom indices.
    #
    method ScrollUp {} {
	
	# If the top index is 0, we need an up arrow at the top:
	# and must delete an entry to keep the menu size constant.
	#
	if {$topIndex == 0} {
	    $hull insert 0 command -image $uparrow
	    $hull delete 1 
	}
	$hull delete 1;		# Drop the top functional item.
	
	# Add the next unseen item:

	incr topIndex
	incr bottomIndex
        set  newBottomItem $items($bottomIndex)
	set  menuIndex [expr {[$hull index last]}]

	set itemType [dict get $newBottomItem type]
	set itemOptions  [dict get $newBottomItem options]


	$hull insert $menuIndex $itemType {*}$itemOptions

	# If the new bottom item is the last one, remove the
	# bottom scroll arrow:

	if {$bottomIndex == ($itemCount -1)} {
	    $hull delete last
	}
	# Ensure the last item remains active:

	$hull activate last

    }
    ##
    # Scroll the  menu down.  This means:
    #   - If necessesaary add a scroll arrow to the bottom.
    #   - Remove the next to the last mene entry.
    #   - adjust the top/bottom indices.
    #   - Add a new menu item at position 1.
    #   - If necessary kill the top arrow.
    # 
    method ScrollDown {} {
	
	# If the bottom index is the last element
	# we  must add a scrolling arrow to the bottom of the menu.

	if {$bottomIndex == ($itemCount - 1)} {
	    $hull add command -image $downarrow
	}
	
	# Remove the next to last elementL:


	set lastItem [expr {[$hull index last]} -1]
	$hull delete $lastItem

	# Adjust the indices and push in a new item at the top of the menu:

	incr bottomIndex -1
	incr topIndex    -1
	set newItem     $items($topIndex)
	set itemType    [dict get $newItem type]
	set itemOptions     [dict get $newItem options]

	$hull insert 1 $itemType {*}$itemOptions
	
	# If topIndex is 0, kill the arrow:
	# And add yet another entry... if there is one:

	if {$topIndex == 0} {
	    $hull delete 0
	    if {$bottomIndex < ($itemCount -1)} {
		incr bottomIndex
		set newItem $items($bottomIndex)
		set itemType [dict get $newItem type]
		set itemOptions [dict get $newItem options]
		
		# Safety code in case there's only one item shown (Bug #6154)

		set insindex [expr {max(([$hull index last] - 1), 0)}]

		$hull insert $insindex $itemType {*}$itemOptions
	    }
	}

    }


    #-----------------------------------------------------------------------
    #  Debug methods:
    #

    method dump {} {
	puts "-------- Internal state of $hull ---------------"
	parray items
	puts "itemCount   : $itemCount"
	puts "topIndex    : $topIndex"
	puts "bottomIndex : $bottomIndex"
	puts "scrolling   : $scrolling"
    }
      
}
