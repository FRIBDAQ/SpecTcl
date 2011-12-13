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
package require treemenuWidget
package require treeParameterEditor
package require treeUtilities

package provide treeParametersContainer 1.0

##
#  Provide a snit megawidget that can contain a set of treeParameterEditor widgets.
#  Editors can be selected via radio buttons on the left of the megawidget.
#  The visual on the editor is as follows:
#
#  +--------------------------------------------------------------------------+
#  | Parameter<mb>       Name       Low    High     Unit            [] Array  |
#  +--------------------------------------------------------------------------+
#  |  ()            |    Editor widget 1                                      |
#  +--------------------------------------------------------------------------+
#  |  ...                                                                     |
#  
#  +--------------------------------------------------------------------------+
#  | ()             | Editor widget n                                         |
#  +--------------------------------------------------------------------------+
#
#  Note that parameter editor widget lines are numbered from 1.              
#  Note that Parmater<mb> is a menu button with a hierchical menu below it..
#
#  OPTIONS
#    -number     - Number of parametr editors to provide.     (static) default is 20.
#    -current    - Currently selected parameter editor [1..n] (dynamcic) starts 1
#    -parameters - List of parameters to put in the menubutton hierarchy (dynamic)
#    -choosecmd  - Command script to run when a parameter is chosen.
#    -loadcmd    - Command script to run when a load button is clicked.
#    -set        - Command script to run when a set button is clicked.
#    -change     - Command script to run when a Change Spectra button is clicked.
#    -array      - Boolean that indicates/changes the array checkbutton.
#
#  METHODS
#    get ?n?     - Returns the contents of the editor widget  n or if omitted
#                  the currently selected one.  This is returned as:
#                  [list name low high units]
#    load n name low high units
#                - Loads the contents of selection n if n is 'current' the selected editor is loaded.
#    loadcurrent name low high unts
#                - Load the one selected by the radio button set.
#
#  ACTION SCRIPT SUBSITUTIONS:
#     %W  - provides our widget name.
#     %I  -  provides the widget name of the menu name for
##           -choosecmd and the specific editor widget for load/set/change scripts.
#     %L  -  only for -choosecmd  provides the terminal label.
#     %N  -  only for -choosecmd  provides the path to the terminal label selected.
#     %S  - Provides the slot for the button clicks.
#
snit::widget treeParametersContainer {
    hulltype ttk::frame
    option -number     -default 20
    option -current    -default 1
    option -parameters -default [list] -configuremethod updateParameterMenu
    option -choosecmd   -default [list]
    option -loadcmd    -default [list]
    option -set        -default [list]
    option -change     -default [list]
    option -array      -default false

    variable menuButton;	# The button that is attached to the hierarchy menu.

    ##
    #  Construct the widget:
    #  @args - a list of name value options pairs.
    constructor args {

	# Create the widgets along the to of the container.  These are 
	# the menubutton, the titles and the array check box.
	# We can grid those right away.

	set menuButton [ttk::menubutton $win.menu -text Parameter -menu $win.menu.pulldown]
	grid $menuButton -row 0 -column 0

	$self configurelist $args; # Build the menu hierarchy.

	# Now build the radiobutton column and the column of editors.
	# the radio button lives in column 0 while the editors span columns 1 -5 inclusive.

	for {set i 1} {$i <= $options(-number)} {incr i} {
	    set title false
	    set rowspan 1
	    set editorRow $i
	    if {$i == 1} {
		set title true
		set rowspan 2
		incr editorRow -1
	    }
	    ttk::frame $win.b$i -relief ridge -borderwidth 1
	    ttk::radiobutton $win.b$i.b$i -variable ${selfns}::options(-current) \
		-value $i
	    pack $win.b$i.b$i

	    treeParameterEditor $win.e$i -title $title \
		-loadcmd    [mymethod ButtonClicked %W $i -loadcmd] \
		-setcmd     [mymethod ButtonClicked %W $i -set]     \
		-changecmd  [mymethod ButtonClicked %W $i -change]

	    grid $win.b$i -row $i -column 0 -sticky ew -pady 0
	    grid $win.e$i -row $editorRow -column 1 -rowspan $rowspan -sticky ewns -pady 0
	}

	#  This next bit is rather dirty. We want to insert the array check button
	#  in the right most column of the top editor ($win.e1).  This 
	#  ensures it won't hang out over the end of the editors

	ttk::frame       $win.e1.arrayframe -relief ridge -borderwidth 2
	ttk::checkbutton $win.e1.arrayframe.array -text Array \
	    -offvalue false -onvalue true -variable ${selfns}::options(-array) 
	pack $win.e1.arrayframe.array
	grid $win.e1.arrayframe -row 0 -column 6 -sticky e


	# If not supplied we need to build by hand:

	if {$options(-parameters) eq ""} {
	    $self updateParameterMenu -parameters [list]
	}

    }

    # Public methods:

    # configuration methods:


    ## 
    # Configuration method for the -parameters option
    # kill off the existing hierarchical menu and build a new one from the set of parameters
    # we have.
    # @param option - the name of the option being configured (-parameters unless we change that).
    # @param value  - New value for that option, a list of the parameters to put in the menu hierarchy.
    #
    method updateParameterMenu {option value} {
	set options($option) $value

	destroy $menuButton.pulldown

	treeMenu $menuButton.pulldown -items $value -splitchar . \
	    -command [mymethod ParameterChosen %W %L %N]

    }

    # Private methods

    ##
    # Get the contents of an entry from the widget.
    # @param n (default blank) if provided this is the slot to fetch.
    #          if not, the current slot is fetched.
    # @return list
    # @retval [list name low high unit]
    #
    # Assumptions: the editor $i is named $win.e$i
    #
    method get {args} {
	set slot $options(-current); # Default slot
	if {[llength $args] > 0} {
	    set slot [lindex $args 0];	# overridden if provided.
	}
	# Compute the editor name and fetch out the values:

	set editor $win.e$slot
	return [list [$editor cget -name] [$editor cget -low] \
		    [$editor cget -high] [$editor cget -units]]
    }

    ##
    # Set the contents of an entry in the widget.
    # @param slot - The slot to set.
    # @param name - The name to set there.
    # @param low  - The low value.
    # @param hi   - The high value.
    # @param units - The units field.
    #
    method load {slot name low hi units} {
	# Compute the editor widget name from the slot number:

	set editor $win.e$slot
	
	# Load the values:

	$editor configure -name $name -low $low -high $hi -units $units
    }
    ##
    # Same as load above except that the currently seleted slot is the one that is
    # loaded:
    # 
    # @param name - The name to set there.
    # @param low  - The low value.
    # @param hi   - The high value.
    # @param units - The units field.
    #

    method loadcurrent {name low hi units} {
	set slot $options(-current)
	$self load $slot $name $low $hi $units
    }
    
    ##
    # Event handler for choosing a parameter.  This dispatches to the script in the -choosecmd
    # option.  The following substitutions are handled:
    #  - %W -> win
    #  - %I -> Menu widget name
    #  - %L -> Menu label.
    #  - %N -> Full path to menu item.
    #
    # @param widget - Menu widget name.
    # @param label  - Label on menu that was chosen.
    # @param path   - Full path to menu label.
    #
    method ParameterChosen {widget label path} {
	::treeutility::dispatch $options(-choosecmd) [list %W %I %L %N] [list $win $widget $label $path]

    }

    ##
    #  event handler for any button click in an editor.
    #  The following substitutions are handled:
    #   %W -> $win
    #   %I -> The editor that was clicked (initiator).
    #   %S -> The slot whosse button was clicked (1 - -number).
    # @param editor widget name.
    # @param slot   Number of the editor that was clicked.
    # @param option that has the script to which we must dispatch:
    #
    method ButtonClicked   {editor slot option} {
	::treeutility::dispatch $options($option) [list %W %S %I] [list $win $slot $editor]
    }


}