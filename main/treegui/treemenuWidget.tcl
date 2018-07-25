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
package require scrollingMenu

package provide treemenuWidget 1.0

##
#  Provide a snit megawidget that can represent a menu of a tree of names.
#  The hull of this is the top level menu.  This allows the menu to be
#  bound to menubuttons or any other thing that can post it.
#
# OPTIONS:
#   -pullright   - If true, cascades to the next level are pullright in that no click is required
#                  (done by binding <Motion> to postcascade @%y).
#   -items       - A set of items that make up the tree.
#   -splitchar  - The character that splits the levels of the hierarchy (defaults to .).
#   -command     - Script executed when a terminal menu item is finally selected.
#                  This supports the following substitutions:
#                    %W  - The menu widget.
#                    %L  - The label associated with the menu that was clicked.
#                    %N  - The full pathname of the menu item that was clicked e.g
#                     a->b->c might return a.b.c
#
# METHODS
#   post x y   - posts the top level menu at the specified position.
#  
# NOTE:
#  - No assumption is made about parameter names.  Specifically, the menu path is not constructed
#    from the parameter names as that fails for names like "a ".
#
snit::widgetadaptor treeMenu {


    option -pullright true
    option -items     [list]
    option -splitchar "."
    option -command    [list]

    delegate method * to hull
    delegate option * to hull



    constructor args {
	installhull using scrollingMenu -tearoff 0
	$self configurelist $args


	# Take all of the names and convert them into lists that represent the widget path

	set separatedNames [list]
	set time [time {
	foreach name $options(-items) {	    lappend separatedNames [split $name $options(-splitchar)]
	}
	}]

	set time [time {$self buildSubMenus $win $separatedNames ""}]
    }
    ##
    # public method to dispatch a menu click
    # @param label - the label on the terminal node.
    # @param path  - the full label path to the terminal node...created by rejoining
    #                the elements of the path list with -splitchar.
    #               
    method dispatch {label path} {
	::treeutility::dispatch $options(-command) [list %W %L %N] [list $win [list $label] [list $path]]

    }

    ##
    #  buildSubMenus
    #    Given a menu and a prefix text adds commands and empty cascades to that menu
    #    based on the names passed in.
    #    *   If a name is not a child of the base name, it is ignored.
    #    *   If a name has no children, it is added as a menu item.
    #    *   If a name has children, it is added as an empty cascade with a command to build the
    #        cascade contents on demand.
    #
    #
    #   This is used because for very large menus it can be time consuming to build the menu
    #   all at once.  This incremental build is very much like the incremental build of the folder
    #   gui.
    #
    #  @param menu  - parent menu widget.
    #  @param names - The names as a list of path lists.
    #  @param prefix- The prefix for which the menu is being built (all added items must be
    #                 for children of prefix.
    #
    method buildSubMenus {menu names prefix} {

		set c 0
	
		#
		# If the menu already has stuff in it we can just return.
		#
		if {[$menu index end] ne "none"} {
			return 
		}
	
		set parentLen [llength $prefix]; # 0 if this is top level.
	
	
		# Compute indices for the prefix, the first element after the prefix
		# and the tail.  Note that if the prefix is "" the prefix is empty:
		#
	
		if {$parentLen == 0} {
			set prefStart -1
			set prefEnd   -1
			set chStart    0
			set tailStart  1
		} else {
			incr parentLen -1
			set  prefStart 0
			set  prefEnd   $parentLen
			
			incr parentLen
			set  chStart $parentLen
	
			set tailStart [incr parentLen]
		}
		
		
		#
		#  Build an array whose indices are prefix immediate children
		#  and whose contents are all children of that child.  
		#
	
		array set children [list]; # Empty array of children lists:
	
		foreach path $names {
			set head [lrange $path $prefStart $prefEnd]
			if {$head eq $prefix} {
			set child [lindex $path $chStart]
			set tail  [lrange $path $tailStart end]
			lappend children($child) $tail
			}
		}
		#  indices of children must have entries:
		#  *  Those with only one element in their lists are terminal nodes
		#  *  Those with more than one element in their lists are parents of the next level:
		
		set cascades     [list]
		set cascPrefixes [list]
		foreach child [lsort -dictionary -increasing [array names children]] {
			if {([llength $children($child)] == 1) && ([lindex $children($child) 0] eq "")} {
				set path [concat $prefix $child [lindex $children($child) 0]]
				$menu add command -label $child \
					-command [mymethod dispatch $child [join $path $options(-splitchar)]]		
			} else {
				set cascmenu [scrollingMenu $menu.c[incr c] -tearoff 0]; # New menu for cascade (not yet stuffed);
				$menu add cascade -menu $cascmenu -label $child -command [mymethod buildSubMenus $cascmenu $names [concat $prefix $child]]
				lappend cascades $cascmenu
				lappend cascPrefixes [concat $prefix $child]
				if {[info globals ScrollRate] eq "ScrollRate"} {
					$cascmenu configure -scrolltimer $::ScrollRate
				}
			}
		}
		# If there are cascades we must
		# - Bind a motion event on this menu so that it builds the contents of the submenus.
		# - posts any cascade near the pointer.
	
		if {([llength $cascades] > 0) && $options(-pullright)} {
			bind $menu <Motion> [mymethod buildCascades  $cascades $names $cascPrefixes]
			bind $menu <Motion> +[list $cascmenu postcascade @%y]
			
		}

    }
    ##
    #  buildCascades
    #    For a set of cascade menus buid the contents of the submenu.
    #
    # @param cascades     - List of cascade menu widgets.
    # @param names        - The full name set.
    # @param prefixes       - The prefixes used to populate the children.
    #
    method buildCascades {cascades names prefixes} {
	foreach cascade $cascades prefix $prefixes {
	    $self buildSubMenus $cascade $names $prefix
	}
    }

}
