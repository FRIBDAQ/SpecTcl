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
package require Itcl
package require treeVariableContainer
package require guistate;	# From 'folder gui'.
package require restore

package provide variableTabActions 1.0


##
#  Provides the behavior associated with the tree variables editor tab.
#  OPTIONS
#   -widget  - specifies the widget name of the treeVariableContainer that is the
#              view.  Constructing this class creates the widget but it is up to the
#              caller to determine how to manage it.
#

itcl::class variableTabActions {
    
    public variable widget 
    public variable lines 20
    



    #-------------------------------------------------------------------
    # Callbacks 


    ##
    # Observer method to save the layout of the editors.
    # This is called as part of the process of writing a configuration file.
    #
    # @param fd - file descriptor open on the configuration file.
    #
    # @note The layout is saved in an array named variable with the following indices:
    #       - Namei  - The name in the i'th slot of the editor.
    #       - Valuei - The value in the i'th slot  of the editor.
    #       - Uniti  - The untis of the i'th slot of the editor.
    #       - Arraty - The state of the array checkbutton.
    #       - select - The editor line that is currently selected.
    #
    private method saveLayout fd {

	puts $fd "\n#-- Variable tab layout\n";
	
	# Save the lines that have a non-blank name:

	set lines [$widget cget -lines]
	for {set i 1} {$i <= $lines} {incr i} {
	    set info [$widget getEditor $i]
	    set name [lindex $info 0]
	    if {$name ne ""} {
		set value [lindex $info 1]
		set units [lindex $info 2]

		puts $fd "set variable(Name$i) [list $name]"; # List will handle names with spaces etc.
		puts $fd "set variable(Value$i) $value"
		puts $fd "set variable(Unit$i) [list $units]"; # Handles e.g. m / s as well as m/s.
		
	    }
	}
	# Now the selected and array states:

	puts $fd "set variable(select) [$widget cget -current]"
	puts $fd "set variable(Array)  [$widget cget -array]"
    }

    ##
    # Clear any layout variables that are lying around prior to a restore.
    #
    private proc clearLayoutVariables {} {
	if {[array exists ::variable]} {
	    unset ::variable
	}
    }
    ##
    # Observer called after a save file is restored.  This
    # restores the layout from the variable array.
    # See saveLayout for the indices in that array.
    #
    private method restoreLayout {} {
	
	# First restore the contents of the editors:

	set lines [$widget cget -lines]
	for {set i 1} {$i <= $lines} {incr i} {
	    if {[array name ::variable Name$i] eq "Name$i"} {
		$widget loadEditor $i $::variable(Name$i) $::variable(Value$i) $::variable(Unit$i)
	    } else {
		$widget loadEditor $i "" "" "";	# Empty the line.
	    }
	}
	# Set the selection and the array checkbox. 
	# Being lazy here using catch in case the array elements don't exist.

	catch {$widget configure -current $::variable(select)}
	catch {$widget configure -array   $::variable(Array)}
	
    }

    ##
    # Called when a variable is selected from the tree menu,
    # The variable is loaded into the currently seleted editor.
    # @param name - tree variable name.
    # @param index - Current editor number.


    private method LoadVariable {path index} {
	set definition [treevariable -list $path]
	if {[llength $definition] != 0} {
	    set definition [lindex $definition 0]
	    $widget loadEditor $index [lindex $definition 0] [lindex $definition 1] [lindex $definition 2]
	}
    }

    ##
    # Set new tree variable specifications.
    # @param name  - Name of the tree variable.
    # @param value - New value.
    # @param units - New units.
    #
    private method SetVariable {name value units} {

	# Require the value to be a double:

	if {![string is double -strict $value]} {
	    tk_messageBox -icon error \
		-message "Attempted to set $name to  '$value', value must be a valid number" \
		-type ok
	    return
	}

	# Get the correct set of variables to modify depending o the state of the array checkbox.

	if {[$widget cget -array]} {

	    set names [::treeutility::listArrayElements $name [itcl::code $this treeVariableNames]]
	} else {
	    set names [list $name]

	}

	# Loop over the names we need to process.

	foreach name $names {

	    # The tree parameter must exist:
	    set definition [treevariable -list $name]
	    if {[llength $definition] != 0} {
		treevariable -set $name $value $units
		treevariable -firetraces $name
	    }
	}
	[autoSave::getInstance] failsafeSave

    }
    ##
    # Return a list of the tree  variable names.
    # @return list
    # @retval list of tree variable names.

    private method treeVariableNames {{pattern *}} {
	set result [list]

	foreach variable [treevariable -list $pattern] {
	    lappend result [lindex $variable 0]
	}

	return $result
    }
    ##
    # Save the tree parameters to file.
    # @param name - nameof file in which to save the tree parameters.
    #
    private method SaveVariables name {
	set fd [open $name w]
	
	# Put a timestamp in and then invoke writeTreeVariables to do the rest.

	puts $fd "# File written [clock format [clock seconds]]"
	::writeTreeVariables $fd

	close $fd
    }
    ##
    # Restore the tree parameters.  This is really sourcing a tcl script.
    # The script is sourced at the global level.
    # @param name -name of the file to restore.
    # @note  as with the prior gui implementation, this does not reload the values
    #        of any of the editor slots.
    #
    private method RestoreVariables name {
	uplevel #0 source $name
	[autoSave::getInstance] failsafeSave

    }
    ##
    # A tree variable name field has changed.  If the current value matches
    # a known tree variable the editor is loaded with its current value
    # and units..otherwise the value/units are loaded with ?'s.
    #
    # @param index - Index of the editor that changed.
    # @param name  - New value of the name field.
    #
    private method NameChanged {index name} {
	set info [treevariable -list $name]
	if {[llength $info] > 0} {
	    set info [lindex $info 0]
	    set value [lindex $info 1]
	    set units [lindex $info 2]

	    $widget loadEditor $index $name $value $units
	} else {
	    $widget loadEditor $index $name ? ?
	}
    }


    #--------------------------------------------------------------------
    # public interface

    ## 
    #  Construct us.
    # @param args - set of optino values to use in constructing us.
    #
    constructor args {
	eval configure {*}$args;	# set the widget name and lines.

	if {$widget eq ""} {
	    error "The -widget option is mandatory"
	}

	treeVariableContainer $widget -lines $lines  -selectcmd [itcl::code $this LoadVariable %N %I] \
	    -variables [treeVariableNames] \
	    -loadcmd   [itcl::code $this LoadVariable %N %I] \
	    -setcmd    [itcl::code $this SetVariable %N %V %U] \
	    -savefile  [itcl::code $this SaveVariables %F] \
	    -loadfile  [itcl::code $this RestoreVariables %F] \
	    -namechanged [itcl::code $this NameChanged %I %N]

	# Add observers for save and restore so that we can 
	# save our gui state.

	addSaveObserver variableTabLayout [itcl::code $this saveLayout]
	[Restore::getInstance] addPreObserver variableTabLayout [itcl::code clearLayoutVariables]
	[Restore::getInstance] addObserver    variableTabLayout [itcl::code $this restoreLayout]
    }


}
