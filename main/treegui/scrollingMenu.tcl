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
    variable visible      0;    # True if mapped false if not.
    variable idlewait     0;    # So we can wait synchronously for idle.

    # Our extended options:

    option -scrolltimer 250;	# ms between successive scrolls.
    option -minvisible   10;    # Only effective at post time.

    # Unless overridden, all the methods and options are passed through to the underlying
    # menu.

    delegate option * to hull
    delegate method * to hull


    constructor args {
	installhull using menu

	$self configurelist $args


	#  Important safety tip here -- why the after:
	#  It turns out that the menu widget figures out what to do on when an entry
	#  is clicked _after_ the unmap event is handled.  It does this by knowing
	#  the index of the item that was clicked and then asking that item what its
	#  -command option was.   However our <Unmap> handler destroys the menu entries
	#  which, in turn, removes the ability of the click handler to know which
	#  command to call (yes it really does this and it took me a good hour to
	#  figure that out when the after was not present -- as of Tk 8.5.11).
	#  By scheduling _Unmap to happen we work around that 'little' ordering problem.

	bind $win <Unmap> +[list after idle [mymethod _Unmap]]

	#  This is done with an after idle to force an ordering between
	#  unmap and map handling.  Otherwise we may handle a second map prior
	#  to handling the first unmap
	
	bind $win <Map>  +[list after idle [mymethod _Map]]
    }
    destructor {
	bind $win <Map> ""
	bind $win <Unmap> ""
	
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


	if {$visible} {
	    $self _addItemToMenu $itemType {*}$args
	}

	# regardless store the item in the array:
	#

	set items($itemCount) [dict create type $itemType options $args]	
	incr itemCount;	
    }
    #-----------------------------------------------------------------------------
    #
    # Private methods:

    ##
    # _itemHeight 
    #
    #  Return out best guess as to the height of the next menu item.
    #  If there are no menu items, an arbitrary, approximate value is
    #  used.
    #  If there are menu items the window ht divided by the
    #  number of items is the average item height.
    #
    #  @return integer - number of pixels in the height of a menu item.
    #
    method _itemHeight {} {
	set nVisible [expr {$bottomIndex - $topIndex}]
	if {$scrolling} {incr nVisible}; # The arrow counts too.

	if {$nVisible == 0} {
	    set lastHt 0;	# Window has no height now.
	    return 30;		# Some arbitrary height
	} else {
	    set ht [winfo height $win]
	    set itemHt [expr {$ht/$nVisible + 1}];         # Windows off by one...
	    return $itemHt
	}
	
		      
    }
    ##
    # _itemFits
    #   Determines if an new item will fit on the screen.
    #   -  If the window size + itemHt will fit
    #      on the screen all done, return true.
    #   - If not there are two cases:
    #     * If we have -minvisible items or more,
    #       we return false
    #     * If we don't have -minvisible items,
    #       we repost the menu upwards by the height of
    #       an item and a bit more and then return true.
    #       If that would drive the y origin of the menu off the
    #       top of the screen we return false...hopefully
    #       this special case does not happen as that would imply
    #       either a gi-humongous font or a really tiny screen.
    #       in the y direction.
    #
    #
    #  @param x0 - the x coordinate at which the menu is posted.
    #  @param y0 - the y coordinate at which the menu is posted.
    #  @param itemHt - Best guess as to the height of a menu item.
    #  @return bool - True if we don't need to initiate scrolling
    #                 for a newly added item.
    #
    method _itemFits {x0 y0 itemHt} {
	
	#  If we're scrolling we already know this won't fit:
	
	if {$scrolling} {
	    return false
	}
	
	set screenHt [winfo vrootheight $win]; # possible . and us are on different screens.
	
	# Seems like we need a bit of tolerance on the screen ht:

	incr screenHt -20

	set currentHt [winfo height $win]
	
	# Bottom of the window if we just extend it by itemht:

	set newBottom [expr {$y0 + $currentHt + $itemHt}]

	# Let's run through the cases -- it fits:

	if {$newBottom < $screenHt} {
	    return true
	}

	# Do we need to move the window to make -minvisible fit?

	set min $options(-minvisible)
	set nVisible [expr {$bottomIndex - $topIndex}]
	if {$scrolling} {incr nVisible}; # Pathological if true but...
	
	# If necessary repost the menu:
	

	if {$nVisible < $min} {
       
	    # Figure out a y0 that puts newbottom on the map:
	    # Note we tried...we really tried to just incrementally
	    # move y0 -- but evidently there's sufficient delay
	    # between posting and the post being active that did not
	    # work.
	    #  This works if we got multiple items off the bottom
	    # of the screen before the window actually moved.
	    
	    set dy [expr {$screenHt - $newBottom}]; # Want it negative.
	    incr y0 $dy

	    if {$y0 < 0} {set y0 0}
	    $win post $x0 $y0
	    
	    if {$y0 > 0} {
		return true
	    } else {
		return false;	# Could not fit min items on screen.
	    }
	}
	# Can't fit and have the minimum set:
	return false
	    
    }
    ##
    # _synchronize
    #   Wait for all pending UI events to be run.
    #
    method _synchronize {} {
	update idletasks
	after idle [list incr [myvar idlewait]]
	vwait [myvar idlewait]
    }
    ##
    # _addItemToMenu
    #   Actually adds an item to the underlying menu object.
    #   This is a bit complex as we want to ensure that we do
    #   have a nonempty menu (not just a scroll arrow. 
    #   Here's a rough overview of what we'll do:
    #   -  If there are at least two  menu entries we'll use the size of the
    #      menu to figure out how big each menu item is and use that
    #      in the rest of the computations.   If there's only 1 entry
    #      we'll use the size of the window to figure out the menu size.
    #      If there are no entries we'll use an arbitrary 30 pixels 
    #      as the height of a menu item.
    #   -  If the item fits on the screen, we'll just add it.
    #   -  If the item does not fit on the screen and we have fewer than
    #      -minvisible we repost the menu up by a couple of menu entry widths
    #      to ensure that we can fit at least -minvisible items on the screen.
    #   -  If on the other hand the menu entry does not fit, but we already
    #      have 10 items, we'll repost a menu entry up and initiate
    #      scrolling
    #
    # @param itemType -- the type of item to add, e.g. cascade or command.
    # @param args     -- The remaining menu parameters as a list (will be passed
    #                    to menu add as {*}$args.
    #
    method _addItemToMenu {itemType args} {

	#  This after idle is necessary;  The actual window posting and
	#  resizing is asynchronous and in the event queue.  Since I'm
	#  relying on the window to be posted and sized/positioned as per
	#  our last call, we need to ensure that all of the queued events
	#  are executed.   Hence the code below.  The identical code at the
	#  end of this proc method is a just in case we probably don't need
	# but this was tough enough as is.

	set labelIndex [expr {[lsearch -exact $args -label] + 1}]
	set label [lindex $args $labelIndex]

	$self _synchronize
	if {!$visible} {
	    return
	}

	set lIdx [lsearch -exact $args -label]
	incr lIdx


	# figure out where the menu is posted:

	set x0 [winfo rootx $win]
	set y0 [winfo rooty $win]


	# Figure out the ht of a menu item:

	set itemHt [$self _itemHeight]

	# If the item fits add it else initiate scrolling.


	if {[$self _itemFits $x0 $y0 $itemHt]} {

	    $hull add $itemType {*}$args
	    incr bottomIndex

	} else {

	    $self StartScrolling

	}

    }

    ##
    # _Map
    #   Handler for when the menu is mapped to the screen.
    #   set visibility to 1 and stock the menu.  Note the
    #   menu items are ordered:
    #
    method _Map {} {
	set visible 1
	set topIndex 0
	set bottomIndex 0
	for {set i 0} {$i < $itemCount} {incr i} {
	    set item $items($i)
	    set type [dict get $item type]
	    set opts [dict get $item options]
	    $self _addItemToMenu $type {*}$opts
	}
    }
    ##
    # _Unmap
    #  Handler for when the menu is unmapped.
    #  destroy all items in the menu - the submenus are left alone:
    #
    method _Unmap {} {
	$hull delete 0 end
	set visible 0
	set scrolling 0;    # No longer scrolling if not visible.
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

	
	bind $win <Enter>           +[mymethod ScrollMenu]
	bind $win <ButtonRelease-1> +[mymethod CancelScrollingTimer]
	bind $win <Leave>           +[mymethod CancelScrollingTimer]


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

	if {!$visible} return
	
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
	if {!$visible} {
	    return
	}
	
	# Protect against scrolling up too far:
	if {$bottomIndex >= $itemCount} {return}
	
	# If the top index is 0, we need an up arrow at the top:
	# and must delete an entry to keep the menu size constant.
	#
	if {$topIndex == 0} {
	    $hull insert 0 command -image $uparrow
	    $hull delete 1 
	}

	$hull delete 1;		# Drop the top functional item.
	
	# Add the next unseen item ... if there are any left:

	incr topIndex
	incr bottomIndex
        set  newBottomItem $items($bottomIndex)
	set  menuIndex [expr {[$hull index last]}]

	set itemType [dict get $newBottomItem type]
	set itemOptions  [dict get $newBottomItem options]

	if {$menuIndex eq "none"} {
	    set menuIndex end
	}
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

	if {!$visible} {
	    return
	}
	# Protect against overscrolling:
	
	if {$topIndex <= 0} {return}
	
	# If the bottom index is the last element
	# we  must add a scrolling arrow to the bottom of the menu.

	if {$bottomIndex == ($itemCount - 1)} {
	    $hull add command -image $downarrow
	}
	
	# Remove the next to last elementL:

	set lastItem [expr {[$hull index last]} -1]
	if {$lastItem != -1} {
	    $hull delete $lastItem
	}

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

		# Should not be able to insert at -1 but protect anyway:

		set insidx [expr {max(([$hull index last] -1), 0)}]

		$hull insert $insidx  $itemType {*}$itemOptions
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
