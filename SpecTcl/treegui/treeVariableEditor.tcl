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
#    -namechanged - Script invoked if the name field of one of the editors changed.
#
# METHODS:
#   loadEditor - Loads the contents of a specific editor.
#   getEditor  - Get the contents of a specific editor.
#
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
    option -namechanged -default [list]

    delegate option * to hull

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
	    ttk::entry       $win.name$row  -width 32 -takefocus 1
	    ttk::entry       $win.value$row -width 10 -takefocus 1
	    ttk::entry       $win.units$row -width 10 -takefocus 1
	    ttk::button      $win.load$row  -text Load -command [mymethod ReloadDispatch $row]
	    ttk::button      $win.set$row   -text Set  -command [mymethod SetVariable $row]

	    grid $win.radio$row $win.name$row $win.value$row $win.units$row $win.load$row $win.set$row \
		-sticky new

	    # Bindings for this row as well.. note that tab/shift tab normally change focus.

	    foreach binding [list  <Return> ] {
		foreach \
		    widget [list $win.name$row $win.value$row $win.units$row] {
			bind $widget $binding [mymethod changeFocus tk_focusNext %W]
		    }
	    }
	    # Currently no left movement bindings other than the default, however
	    # if someone suggests them they can be added to the list here.
	    #
	    foreach binding [list] {
		foreach \
		    widget [list $win.name$row $win.value$row $win.units$row] {
			bind $widget $binding [mymethod changeFocus tk_focusPrev %W]
		    }
	    }
	    # The after in this binding allows the entry to change so a get
	    # of the value reflects the contents after the keystroke.
	    #
	    bind $win.name$row <Key> [list after idle  [mymethod Keystroke $row]]
	}
	foreach column [list 0 1 2 3 4 5] weight [list 0 20 1 1 0 0] {
	    grid columnconfigure $win $column -weight $weight
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
	   setEntryValue $widget $value
	}
    }
    ##
    # Return a list of the values in a specific editor:
    #
    # @param editor - index of the editor.
    #
    # @return list
    # @retval [list name value units] 
    #
    method getEditor editor {
	foreach widget [list $win.name$editor $win.value$editor $win.units$editor] {
	    lappend result [$widget get]
	}
	return $result
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
	    $self Dispatch $options(-setcmd) [list %N %V %U %I %W] \
		[list $name [list $value] [list $units] $index $win]
	}
    }
    #---------------------------------------------------------------------
    # Private utilities.
    #
    ## 
    # Change the focus:
    #    @param nextcmd - command that determines the next widget given the current widget.
    #    @param widget  - Current widget.
    #
    method changeFocus {nextcmd widget} {

	# after since entry widgets do immediate focus games with some chars.

	after idle  [list focus [$nextcmd $widget]]
    }

    ##
    #  Dispatch a name change callback:
    # @param row - The row of the editor that had a name change.
    #
    # Substitutions:
    #    - %W  - This megawidget.
    #    - %I  - The row of the widget that was modified.
    #    - %N  - The contents of the name after the stubstition.
    #
    method Keystroke row {
	set entryWidget $win.name$row

	$self Dispatch $options(-namechanged) [list %W %I %N] [list $win $row [list [$win.name$row get]]]
    }

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
    ##
    # Utility to set the text of an entry widget while preserving the 
    # cursor position
    #
    # @param widget  - Text widget.
    # @param text    - New text.
    #
    # @note if the text is shorter than the current cursor position, the cursor
    #       will be put at the end of the text.
    #
    proc setEntryValue  {widget text} {
	
	#  Get the current cursor position:

	set cursorPosition [$widget index insert]
	$widget delete 0 end
	$widget insert 0 $text

	# Figure out if the cursor position needs to be limited by the text:

	set textLength [string length $text]
	if {$cursorPosition > $textLength} {
	    set cursorPosition $textLength
	}
	$widget icursor $cursorPosition
    }

}