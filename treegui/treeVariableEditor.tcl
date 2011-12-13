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
package require treeUtilities

package provide treeVariableEditor 1.0

##
# Provides a snit megawidget that contains a bunch of tree variable
# editors.  The number of editors is configurable at start time.
# The layout of the widget is as follows:
#
# +-----------------------------------------------------------------+
# |  [Variable]      Name     Value    Unit     [ ] Array           |
# \                                                                 \
# /    Editors are here...see below                                 /
# |                                                                 |
# +-----------------------------------------------------------------+
#
# Each editor take up one line and looks like this:
#
# +------------------------------------------------------------------+
# |   ()     | [           ]|[       ]|[         ]|[Load] | [Set]    |
# +------------------------------------------------------------------+
#
# OPTIONS:
#    -variables  - List of tree variables to load into the variable pulldown/right menu
#    -lines      - Number of editor lines to display.
#    -current    - Selects/fetches the number of the line currently selected by the radio button.
#    -selectcmd  - Script that is invoked when the variable pulldown/right menu selects
#                  a variable.  Substitutions include the normal substitutions available to 
#                  tree menu widgets (%W - which is our widget, however, %L  menu label.
#                  %N full path to the menu as well as %I - Currently selected editor line.
#    -loadcmd    - script that is invoked when the load button is clicked.
#                  Substitutions include:
#                    -  %N name of variable loaded into the editor.
#                    -  %I Index of the editor that invoked this.
#                    -  %W $win.
#    -setcmd     - Script that is invoked when the Set button is clicked. The following substitutions
#                  are supported:
#                    %N - name loaded into the editor.
#                    %V - Value loaded into the editor.
#                    %U - Units loaded into the editor.
#                    %I - Index of the selected editor.
#                    %W - Widget of the selected editor.
#    -array      - 0 if the array check button is off 1 otherwise.
#
# METHODS:
#   loadEditor - Loads the contents of a specific editor.
# AUTONOMOUS ACTIONS:
#   Tab, Return, Right  navigates circularly on the editor in focus to the right
#   Shift-Tab, Left ISO_Left_Tab navigates circularly on the editor in focus to the left.
#   

snit::widget treeVariableEditor {
    hulltype ttk::frame
    option -variables -default [list] -configuremethod ConfigureVariables
    option -lines     -default 20
    option -current   -default 1
    option -selectcmd -default [list]
    option -loadcmd   -default [list]
    option -setcmd    -default [list]
    option -array     -default 0

    ##
    #Create/layout the widgets and set up the callbacks and bindings.
    #
    # @param args - list of optionname values.

    constructor args {
	# Before configuring anything we need to build the top line so
	# the menu tree exists if -variables is specified.

	ttk::menubutton $win.menu -text "Variable" -menu $win.menu.variables
	treeMenu $win.menu.variables -command [mymethod SelectVariable  %L %N]
	ttk::label $win.name -text  Name
	ttk::label $win.value -text Value
	ttk::label $win.units -text Units
	ttk::checkbutton $win.array -offvalue 0 -onvalue 1 -text Array -variable \
	    ${selfns}::options(-array)

	$self configurelist $args

	grid $win.menu $win.name $win.value $win.units
	grid $win.array -row 0  -column 5
	# Now build the editors. -lines must be alive and happy.

	for {set row 1} {$row <= $options(-lines)} {incr row} {
	    ttk::radiobutton $win.radio$row -value $row -variable ${selfns}::options(-current)
	    ttk::entry       $win.name$row  -width 32
	    ttk::entry       $win.value$row -width 10
	    ttk::entry       $win.units$row -width 10
	    ttk::button      $win.load$row  -text Load -command [mymethod ReloadDispatch $row]
	    ttk::button      $win.set$row   -text Set  -command [mymethod SetVariable $row]

	    grid $win.radio$row $win.name$row $win.value$row $win.units$row $win.load$row $win.set$row

	    # Bindings for this row as well:

	    foreach binding [list <Tab> <Return> <Right> ] {
		foreach \
		    widget [list $win.name$row $win.value$row $win.units$row] \
		    nextwidget [list $win.value$row $win.units$row $win.name$row] {
			bind $widget $binding [list after 2  focus $nextwidget]
		    }
	    }
	    foreach binding [list  <Shift-Tab> <Left> <ISO_Left_Tab> ] {
		foreach \
		    widget [list $win.name$row $win.value$row $win.units$row] \
		    prior  [list $win.units$row $win.name$row $win.value$row] {
			bind $widget $binding [list after 2  focus $prior]
		    }
	    }
	}
    }
    #------------------------------------------------------------------------------
    #  Public methods:

    ##
    # Load an editor with a tree parameter:
    # @param editor - index of the specific editor.
    # @param name   - Name of the tree variable.
    # @param value  - Value of the tree variable.
    # @param units  - Units of the tree variable.
    #

    method loadEditor {editor name value units} {
	foreach widget [list $win.name$editor $win.value$editor $win.units$editor] \
	    value  [list $name $value $units] {
	   $widget delete 0 end
	   $widget insert 0 $value
	}
    }


    #-------------------------------------------------------------------------------
    #
    #  Configuration handlers:
    
    ##
    # Configure the variables in the dropdown/right variables menu.
    # @param option - name of option being configured (-variables).
    # @param value  - New set of variables to display (-items in the treemenu).
    #
    method ConfigureVariables {option value} {
	set options($option) $value

	# Recreate the menu with the new values.

	destroy $win.menu.variables
	treeMenu $win.menu.variables -items $value -command [mymethod SelectVariable %L %N]

    }
    #-----------------------------------------------------------------------
    # GUI event/callback handlers
    #

    ##
    # SelectVariable - Called when a variable menu item has been selected
    # by the user.
    # @param label - Label of the selected item.
    # @param path  - Path to the selected item.
    #
    method SelectVariable {label path} {
	set script $options(-selectcmd)

	$self Dispatch $script [list %W %L %N %I] [list $win $label $path $options(-current)]
    }
    
    ##
    # ReloadDispatch - dispatches the -loadcmd.  See the comment header for the set of 
    # substitutions supported.
    #
    #  @param row - Number of the selected editor.
    #
    method ReloadDispatch row {
	set name [$win.name$row get]

	# Only dispatch if there's a non-empty name:

	if {$name ne ""} {
	    $self Dispatch $options(-loadcmd) [list %W %N %I] [list $win $name $row]
	}
    }
    ##
    # Dispatch the -setcmd script.  See the header comments for the supported substitutions.
    # @param index - Index of the editor that was involved.
    #
    method SetVariable index {
	set name [$win.name$index get]
	if {$name ne ""} {
	    set value [$win.value$index get]
	    set units [$win.units$index get]
	    $self Dispatch $options(-setcmd) [list %N %V %U %I %W] [list $name $value $units $index $win]
	}
    }
    #---------------------------------------------------------------------
    # Private utilities.
    #

    ## 
    # Dispatch to a script at the global level with substitutions:
    #
    # @param script - Script to run (could be empty in which case this is a no-op).
    # @param substs - List of substitions that can be performed on the script,.
    # @pram  values - For each element of substs a value that can be plugged into the script
    #
    method Dispatch {script substs values} {
	::treeutility::dispatch $script $substs $values
    }
    
}