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
#

snit::widget treeVariableEditor {
    hulltype ttk::frame
    option -variables -default [list] -configuremethod ConfigureVariables
    option -lines     -default 20
    option -current   -default 1
    option -selectcmd -default [list]

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
	ttk::checkbutton $win.array -offvalue 0 -onvalue 1 -text Array

	$self configurelist $args

	grid $win.menu $win.name $win.value $win.units
	grid $win.array -row 0  -column 5
	# Now build the editors. -lines must be alive and happy.

	for {set row 1} {$row <= $options(-lines)} {incr row} {
	    ttk::radiobutton $win.radio$row -value $row -variable ${selfns}::options(-current)
	    ttk::entry       $win.name$row  -width 32
	    ttk::entry       $win.value$row -width 10
	    ttk::entry       $win.units$row -width 10
	    ttk::button      $win.load$row  -text Load
	    ttk::button      $win.set$row   -text Set

	    grid $win.radio$row $win.name$row $win.value$row $win.units$row $win.load$row $win.set$row
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
	puts "SelectVariable"
	set script $options(-selectcmd)

	$self Dispatch $script [list %W %L %N %I] [list $win $label $path $options(-current)]
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
	puts Dispatch
	if {$script ne ""} {
	    puts non-null
	    foreach subst $substs value $values {
		regsub -all  $subst $script $value script
	    }
	    puts $script
	    uplevel #0 $script
	}
    }
    
}