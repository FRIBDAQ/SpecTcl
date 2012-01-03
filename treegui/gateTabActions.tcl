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
package require gateContainer

package provide gateTabActions 1.0



##
# Provides an itcl class that encapsulates the behavior of the gates container.
# Construction of the class will instatiate the container widget on the 
# specified .widget but will not make it visible.  Making it visible is the
# client's job.  gate add/delete traces are used to update the gate name menu in the
# create bar..gate table updates are driven by callbacks from the gate mask bar at the
# bottom of the megawidget.
#
# OPTIONS:
#    -widget   - Name of gate container widget to create.
#
::itcl::class gateTabActions {
    public variable widget ""

    private variable gateAddChain;	# prior gate add trace.
    private variable gateDeleteChain;	# prior gate delete trace.
    private variable gateChangeChain;   	# prior gate change trace.

    #------------------------------------------------------------------------------
    # Private methods:

    ##
    # Return the type code of a gate:
    # @param gate - the gate to parse.
    #
    private method gateType gate {
	return [lindex $gate 2]
    }
    ##
    # Updates both the gate table and the gate menu using the mask:
    #
    private method updateGates {} {
	loadGateTable [$widget cget -mask]
	loadGateMenu
	    
    }
    ##
    # The primitive gates need to have their definitions munged a bit
    # when being loaded into the definition string entry.  This is because gate -list
    # provides [list param1 param2]  [list x1 y1] [list x2 y2]...]
    # but the definition requires:
    #  [list param1 param2 [list [list x1 y1] [list x2 y2]...]]
    # @param def - original def
    # @return list
    # @retval - the list that the gate creation command expects.
    #
    method mungGateDefinition def {
	set params [lindex $def 0]
	set coords [lrange $def 1 end]

	set xParam [lindex $params 0]
	set yParam [lindex $params 1]

	set def [list $xParam $yParam]
	lappend def $coords

	return $def
    }

    #-------------------------------------------------------------------------------
    #
    # Callbacks (private)
    #

    ##
    # Load the gate table with the set of gates specified by the mask.
    #
    # @param mask - Glob pattern used to select the gates to modify.
    #
    # @note false gates are not listed as they are assumed to represent deleted gates
    #
    private method loadGateTable mask {
	set gates [list]
	foreach gate [gate -list $mask] {
	    if {[gateType $gate] ne "F"} {
		lappend gates $gate
	    }
	}
	$widget configure -gates $gates
    }

    ## 
    #  The methods below 
    #  Are required to support chaining to prior traces:
    # in all of them:
    #  @name is the name of the gate that fired the trace.
    #
    private method gateAdded name {
	loadGateMenu
	if {$gateAddChain ne ""} {
	    uplevel #0 $gateAddChain $name
	}
    }
    private method gateDeleted name {
	loadGateMenu
	if {$gateDeleteChain ne ""} {
	    uplevel #0 $gateDeleteChain $name
	}
    }
    private method gateChanged name {
	loadGateMenu
	if {$gateChangeChain ne ""} {
	    uplevel #0 $gateChangeChain $name
	}
    }
    ##
    # Load the gate menu with the names of all of the gates:
    #
    private method loadGateMenu {} {
	set gates [gate -list]
	set names [list]
	foreach gate $gates {
	    if {[gateType $gate] ne "F"} {
		lappend names [lindex $gate 0]
	    }
	}
	$widget configure -menugates $names

	# If the name was supplied we chain to any 
    }

    
    ## 
    # Called when a gate is double clicked in the gate table.
    # Ths loads the gate specification into the gate creation widget.
    # @param name - gate name.
    #
    private method loadGateSpec name {
	set gateSpec [gate -list $name]

	if {([llength $gateSpec] != 0)} {
	    set gateSpec [lindex $gateSpec 0]

	    set type [gateType $gateSpec]
	    if {$type == "F"} {

		# Deleted gate

		set def ""
		set type [$widget cget -type]
	    } else {

		set def  [lindex $gateSpec 3]

		# Bands and contours have to be handled differently 
		# Because of the fact that their gate -list is slightly
		# different than their gate definition.
		
		if {$type in {b c}} {
		    set def [mungGateDefinition $def]
		}
	    }

	    $widget configure -gatename $name -type $type -definition $def

	} else {
	    # the gate does not exist for whatever reason:

	    $widget configure -gatename $name -definition ""
	}

    }
    ##
    # Delete the gate(s) that are currently selected in the table.
    # An update using the current mask is also forced.
    #
    private method deleteSelected {} {
	set gates [$widget getsel]
	foreach gate $gates {
	    gate -delete $gate
	}
	updateGates
	[autoSave::getInstance] failsafeSave
    }
    ##
    # Prompt for confirmation and, if we get it, delete all  of the 
    # gates
    #
    private method deleteAll {} {
	set confirmation [tk_messageBox -type yesno -icon warning \
			      -message "Are you sure you want to delete all the gate definitions?" \
			      -default no]
	if {$confirmation == "yes"} {
	    foreach gate [gate -list] {
		#
		# Only bother for those that are already deleted:
		#
		if {[gateType $gate] != "F"} {
		    gate -delete [lindex $gate 0]
		}
	    }
	    updateGates
	    [autoSave::getInstance] failsafeSave
	}

    }
    ##
    # Callback invoked to create/modify a gate.
    # @param name - Name of the gate.
    # @param gateType - SpecTcl gate type.
    # @param definition - Gate definition string.
    #
    #
    private method createGate {name gateType definition} {

	# Require that the gate name, type and definition are not null.
	#
	if {($name eq "" ) || ($gateType eq "") || ($definition eq "")} {
	    tk_messageBox -type ok -message "Gate is not completely defined" -icon error
	} else {
	    if {[catch {gate $name $gateType $definition}]} {
		tk_messageBox -type ok -message "Incorrect gate definition for gate type" -icon error
	    } else {
		updateGates
	    }
	}
	[autoSave::getInstance] failsafeSave
	
    }
    #-------------------------------------------------------------------------------
    #  Public interfaces
    #

    ##
    # construct the megawidget and set up the callbacks/options.
    # Initially the table is loaded with the gates that are defined now
    # As selected by the mask.
    # The gates dropdown is loaded from the names of all gates.
    #
    # @param args - list of option/value pairs
    #
    constructor {args} {
	eval configure $args;	# Ensure the widget name is set:
	if {$widget eq ""} {
	    error "The -widget option is mandator"
	}

	gateContainer $widget -height 20 \
	    -maskcmd          [itcl::code $this loadGateTable %M] \
	    -updatecmd        [itcl::code $this loadGateTable %M] \
	    -command          [itcl::code $this loadGateSpec  %N] \
	    -deleteselected   [itcl::code $this deleteSelected]   \
	    -deleteall        [itcl::code $this deleteAll]        \
	    -createcmd        [itcl::code $this createGate %G %T %D]

	loadGateTable *
	loadGateMenu 

	# Set up a gate add/delete trace to reload the gate menu:


	set gateAddChain    [gate -trace add    [itcl::code $this gateAdded]]
	set gateDeleteChain [gate -trace delete [itcl::code $this gateDeleted]]
	set gateChangeChain [gate -trace change [itcl::code $this gateChanged]]


    }
}