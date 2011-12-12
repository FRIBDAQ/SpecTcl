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
    # Callbacks (note these must be public to work

    ##
    # Called when a variable is selected from the tree menu,
    # The variable is loaded into the currently seleted editor.
    # @param name - tree variable name.
    # @param index - Current editor number.


    public method LoadVariable {path index} {
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
    method SetVariable {name value units} {
	# Get the correct set of variables to modify depending o the state of the array checkbox.

	if {[$widget cget -array]} {

	    set names [::treeutility::listArrayElements $name [list $this treeVariableNames]]
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
    }
    ##
    # Return a list of the tree  variable names.
    # @return list
    # @retval list of tree variable names.

    public method treeVariableNames {{pattern *}} {
	set result [list]

	foreach variable [treevariable -list $pattern] {
	    lappend result [lindex $variable 0]
	}

	return $result
    }
    
    #--------------------------------------------------------------------
    # public interface

    ## 
    #  Construct us.
    # @param args - set of optino values to use in constructing us.
    #
    constructor args {
	eval configure $args;	# set the widget name and lines.

	if {$widget eq ""} {
	    error "The -widget option is mandatory"
	}

	treeVariableContainer $widget -lines $lines  -selectcmd [list $this LoadVariable %N %I] \
	    -variables [$this treeVariableNames] \
	    -loadcmd   [list $this LoadVariable %N %I] \
	    -setcmd    [list $this SetVariable %N %V %U]
    }


}
