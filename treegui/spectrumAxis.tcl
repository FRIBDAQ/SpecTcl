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
package require treemenuWidget

package provide spectrumAxis 1.0

##
# Provides a widget that implements a spectrum axis editor.
# the spectrum axis editor looks like this:
#
#  +---------------------------------------------------------+
#  |  <Parameter ->           Low      High    Bins    Units |
#  | [                   ]  [    ]   [     ]  [    ]   xxx   |
#  +---------------------------------------------------------+
#
# OPTIONS:
#   -parameters - loads the parametesr into the parameter pulldown menu.
#   -parameter  - Value of the paramter entry.
#   -low        - Axis low limit.
#   -high       - Axis high limit.
#   -bins       - Axis bin count.
#   -units      - Units of the parameter.
#   -state      - State of the Parameter, low, high and bins entries.
#   -command    - Script executed when a parameter is selected from the menu.
#   -changed    - Script executed when the parameter name has changed (typing).
#
# @note   The low, high, bins are constrained to hold either an empty string
#   or a valid integer (e.g. string isvalid must hold).
# @note substitutions are:
#   - %W - widget ($self)
#   - %L - Label of menu item clicked. [-command]
#   - %N - Full path to the item selected in the menu hierarchy. [-command]
#   - %T - Value of parameter name entry [-changed]
#

snit::widget spectrumAxis {
    hulltype ::ttk::frame
    
    option -parameters   -default [list] -configuremethod RebuildParameters
    option -parameter    -default ""
    option -low          -default ""
    option -high         -default ""
    option -bins         -default ""
    option -units        -default ""
    option -state        -default normal -configuremethod StateChange

    option -command      -default [list]
    option -changed      -default [list]

    ##
    #  Construct and layout the component widgets.
    # 
    # @param args - option name value pairs that make up the initial
    #               configuration of the widget.
    #
    constructor args {

	# Create the widgets:

	# Top row of stuff:

	ttk::menubutton $win.parametermenubutton -text "Parameter" -menu $win.parametermenu -takefocus 0
	treeMenu        $win.parametermenu -command [mymethod Dispatch -command %L %N]
	ttk::label      $win.lowlabel      -text Low
	ttk::label      $win.highlabel     -text High
	ttk::label      $win.binslabel     -text Bins
	ttk::label      $win.unitslabel    -text Units

	# Bottom row of stuff:

	ttk::entry      $win.parameter    -textvariable ${selfns}::options(-parameter)  \
	    -takefocus 1
	ttk::entry      $win.low          -textvariable ${selfns}::options(-low) \
	    -validate key -validatecommand [mymethod validateAxis %P] -width 7 \
	    -takefocus 1
	ttk::entry      $win.high         -textvariable ${selfns}::options(-high) \
	    -validate key -validatecommand [mymethod validateAxis  %P] -width 7  \
	    -takefocus 1
	ttk::entry      $win.bins         -textvariable ${selfns}::options(-bins) \
	    -validate key -validatecommand [list string is integer  %P] -width 7  \
	    -takefocus 1
	ttk::label      $win.units        -textvariable ${selfns}::options(-units) -width 8

	# Grid the elements:

	grid $win.parametermenubutton $win.lowlabel $win.highlabel $win.binslabel $win.unitslabel
	grid $win.parameter $win.low $win.high $win.bins $win.units
	    

	$self configurelist $args

	bind $win.parameter <Key> [list after idle [mymethod Keystroke]]
    }
    #---------------------------------------------------------------------------------
    # Configuration management

    ##
    # Configuration handler to process the -parameters option.
    # we need to tear down the $win.parametermenu, and rebuild it from the list of
    # parameters passed in.
    # @param option - Option being configured -parameters
    # @param value  - New value for the option.
    #
    method RebuildParameters {option value} {
	set options($option) $value

	destroy $win.parametermenu
	treeMenu        $win.parametermenu -command [mymethod Dispatch -command %L %N] \
	    -items $value
    }
    ##
    # Configuration handler to process widget state changes.  We just pass the configuration
    # on to the widgets the user can manipulate.
    # @param option - Option being configured (-state)
    # @param value  - New option value.
    #
    method StateChange {option value} {
	set options($option) $value

	# Disable/enable focus taking appropriately:

	set focus [expr {($value eq "normal") ? 1 : 0}]

	foreach widget [list $win.parameter $win.parametermenubutton $win.low $win.high $win.bins] {
	    $widget configure -state $value -takefocus $focus
	}
    }

    #--------------------------------------------------------------------------------------
    #  Action handlers
    
    ##
    # Called in response to a keystroke in the parameter name entry widget:
    #
    method Keystroke {} {
	::treeutility::dispatch $options(-changed) [list %W %T] [list $self [$win.parameter get]]
    }

    ##
    ##
    # Called to dispatch a substituted command
    # @param option -the option contaning the script to run.
    # @param label - Label of the menu item clicked.
    # @param name  - Full path to the menu entry clicked.
    # 
    method Dispatch {option label name} {
	::treeutility::dispatch $options($option) [list %W %L %N] [list $self [list $label] [list $name]]
    }

    ##
    # Validation command for axes...a leading minus is allowed as well as a floating point.
    # just a minus is allowed since someone might type in order -1.234
    #
    # @param value - Current value of the entry.
    #
    # @return int
    # @retval 0 - invalid value
    # @retval 1 - valid value.
    #
    method validateAxis value {
	set value [string trim $value]
	if {$value eq "-"} {
	    return 1
	}
	return [string is double $value]
    }
    
}