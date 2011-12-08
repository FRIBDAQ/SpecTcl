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

package provide gateCreate

##
# Provides a megawidget for creating gates.
# The physicsl layout of this widget is shown below:
#
#  +---------------------------------------------------------------+
#  | [Create/Replace]  [Gate Select]   [Clear Dependency] [Slice]  |
#  | <Gate name entry> < gate definition entry                  >  |
#  +---------------------------------------------------------------+
#
#  OPTIONS
#
#    -createcmd  Script that is called when the create/replace 
#                button is clicked.
#    -gatename   Contents of the gate name entry.
#    -definition Contents of the gate definition entry.
#    -type       Current gate type selection from its menu dropdown.
#                Note, the value returned is suitable for use as a gate type
#                in the SpecTcl gate creation command.
#    -typename   English name of the gate type.
#    -gates      List of gate names used to populate the "Gate Select"
#                menu button.
#
# AUTONOMOUS ACTIONS
#    Clicking the "Clear dependency" button will clear the gate definition
#    entry widget.
#
# CALLBACK SUBSTITUTIONS
#    %G  - Contents of the Gate name entry.
#    %T  - SpecTcl Gate type name.
#    %D  - Contents of the gate definition entry.
#

snit::widget gateCreate {
    hulltype ttk::frame

    option -createcmd
    option -gatename
    option -definition
    option -type
    option -typename
    option -gates -configuremethod SetGates 

    # Dictionary whose keys populate the gate type menu
    # button drop down and whose values are the gate types
    # Expected by the SpecTcl gate definition.

    typevariable gateTypes [dict create                  \
			    And              *           \
			    Or               +           \
			    Not              -           \
			    C2Band           c2band      \
			    Slice            s           \
			    Contour          c           \
			    Band             b           \
			    {Gamma Band}     gb          \
			    {Gamma Contour}  gc          \
			    {Gamma Slice}    gs          \
			    {Mask Equal}     em          \
			    {Mask And}       am          \
			    {Mask Not Equal} nm          \
			   ]

    ##
    # build and layout the component widgets.
    #
    #  @param configuration parameters as a list of name value pairs.
    #  e.g. -createcmd createScript -gatename george...
    constructor args {
	
	# In our case we need to create the 
	# widgets before allowing configure to run as -gates will
	# need to operate on the treeMenu attached to the
	# Gate Select manu button.

	ttk::button     $win.create -text Create/Replace -command [mymethod Dispatch -createcmd]
	ttk::menubutton $win.gatesel -text {Gate Select} -menu $win.gatesel.gates
	treeMenu        $win.gatesel.gates -command [mymethod AddDependency %N]
	ttk::button     $win.clear  -text {Clear Definition} -command [mymethod ClearDefinition]
	ttk::menubutton $win.type   -textvariable ${selfns}::options(-typename) \
	    -menu $win.type.typemenu
	set options(-typename) [lindex [dict keys $gateTypes] 0]
	set options(-type)     [dict get $gateTypes $options(-typename)]


	menu $win.type.typemenu -tearoff 0
	set width 0
	dict for {label value} $gateTypes {
	    $win.type.typemenu add command -label $label -command [mymethod SetGateType $label $value]
	    set labelLen [string length $label]
	    if {$labelLen > $width} {
		set width $labelLen
	    }
	}
	$win.type configure -width $labelLen


	ttk::entry $win.name        -textvariable ${selfns}::options(-gatename)
	ttk::entry $win.definition  -textvariable ${selfns}::options(-definition)  -width 32

	#
	#  Layout the widget.
	#

	grid $win.create $win.gatesel $win.clear $win.type -sticky w
	grid $win.name        -row 1 -column 0 -sticky w -padx 3px
	grid $win.definition  -row 1 -column 1 -columnspan 3 -sticky ew


	# Configure the args which in turn will configure some of the widget contents.

	$self configurelist $args
    }
    #-------------------------------------------------------------------
    #  Script dispatchers.
    #
    
    ## 
    # Dispatch to user scripts
    #
    # @param option -the option that holds the script.
    #
    # See the class header for information about the substitutions we support.
    #
    method Dispatch option {
	set script $options($option)

	if {$script ne ""} {

	    # Do the % substitutions:

	    foreach pattern {%G %T %D} \
		value [list [list $options(-gatename)] [list $options(-type)] [list $options(-definition)]] {
		    regsub -all $pattern $script $value script
	    }
	    uplevel #0 $script
	}
    }

    #-------------------------------------------------------------------
    # Configuration handlers.

    ##
    # Set the gates for the $in.gatesel.gates menu.
    # At this point in time this required killing and rebuilding that menu:
    # @param option - Name of the option being configured (-gates)
    # @param value  - new value of the option
    #
    method SetGates {option value} {
	set options($option) $value; #  so cget works.

	destroy $win.gatesel.gates
	treeMenu $win.gatesel.gates -command [mymethod AddDependency %N] -items $value
    }

    #--------------------------------------------------------------------
    #
    # Internal methods that provide autonomous behavior.
    #

    ##
    # Clear the definition string:
    #
    method ClearDefinition {} {
	$win.definition delete 0 end
    }
    ## 
    # Add A dependency from the gate menu to the list.
    # The Definition is treated as a well formed Tcl list and the new gate is lappended to the
    # end of it so that whacky gate names will properly work.
    # @param name - Name of the gate to add
    #
    method AddDependency {name} {
	set current [$win.definition get]
	lappend  current $name

	$win.definition delete 0 end
	$win.definition insert 0 $current
    }
    ##
    # Set the new gate type.
    # This requires us to:
    #  - set new values for options(-typename) 
    #  - set new values for optinos(-type)
    #
    method SetGateType {label value} {
	set options(-typename) $label
	set options(-type)     $value
    }
}