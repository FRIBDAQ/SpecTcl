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
#
#  ACTION SCRIPT SUBSITUTIONS:
#     %W  - provides the widget that invoked the action.  This is the menu hierarhcy for
#           -choosecmd and the specific editor widget for load/set/change scripts.
#     %F  -  provides our widget name
#     %L  -  only for -choosecmd  provides the terminal label.
#     %N  -  only for -choosecmd  provides the path to the terminal label selected.
#
snit::widget treeParametersContainer {
    hulltype ttk::frame
    option -number     -default 20
    option -current    -default 1
    option -parameters -default [list] -configuremethod updateParameterMenu
    option -chooscmd   -default [list]
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
	    ttk::radiobutton $win.b$i.b$i -variable ${selfns}::option(-current) \
		-value $i
	    pack $win.b$i.b$i

	    treeParameterEditor $win.e$i -title $title
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

	treeMenu $menuButton.pulldown -items $value -splitchar . -command [mymethod ParameterChosen %W %L %N]

    }

    # Private methods
    
    method ParameterChosen {widget label path} {}


}