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
	installhull using menu -tearoff 0
	$self configurelist $args


	# Take all of the names and convert them into lists that represent the widget path.

	set separatedNames [list]
	foreach name $options(-items) {
	    lappend separatedNames [split $name $options(-splitchar)]
	}

	$self BuildMenus $win 0 $separatedNames
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
    # private method, this is a recursive proc that builds the menu hierarchy.
    # @param widget - The menu we are filling in.
    # @param level  - the menu level we are building
    # @param names  - list of names that are represented as lists that are the paths to each
    #                 menu item.
    #
    method BuildMenus {widget level names} {

	set submenu 0
	
	# Terminal nodes are those whose list length is the same as level+1.
	# We must recurse for nodes that have listlength > level+1
	# There is a nastly little edge case where we could have a node of the form.
	#  ..  a and nodes of the form .. a b...
	# this is handled correctly (by having a terminal and a cascade node with the same labels.
	#

	# Step 1 - create an array whose indices are the set of meny items we must have at this
	#          level and whose elements are the entries at and below this node.
	#

	array set a [list];	# If this is empty we are done.
	set recurseLength [expr {$level + 1}]

	foreach name $names {	   
	    set indexName [lindex $name $level]
	    lappend a($indexName) $name
	}

	# Step 2 if we have any elements in a
	#        create menu entries for them.
	#        Specifically:
	#         - iterate through the list of elements at that index.
	#         - Make cascade list for all elements that have length > recurseLength.
	#         - If we come across a list exactly recurseLength long make a command element for it.
	#         - If we have any elements in the cascade list, create a cascade element
	#           and recurse to stock it.
	#
	if {[array size a] > 0} {
	    foreach label [lsort [array names a]] {
		set cascade [list]
		foreach path $a($label) {
		    if {[llength $path] == $recurseLength} {
			$widget add command -label $label \
			    -command [mymethod dispatch $label [join $path $options(-splitchar)]]
		    } else {
			lappend cascade $path
		    }
		}
		# If cascade is not empty, create the submenu and add a cascade for it:

		if {[llength $cascade] > 0} {
		    set submenuName $widget.c$submenu
		    menu $submenuName -tearoff 0
		    $self BuildMenus $submenuName $recurseLength $cascade
		    $widget add cascade -label $label -menu $submenuName
		    incr submenu
		}
	    }
	    # If this is a pull right cascade bind <Motion> to post the closest cascde:

	    if {$options(-pullright) } {
		bind $widget <Motion> [list $widget postcascade @%y]
	    }
	} 
    }
    
}
