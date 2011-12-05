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
package require treeParametersContainer

package provide parametersTabActions 1.0

##
#  Provides an itcl class that encapsulates the behavior of the treeParemters container.
#  Construction of this class will construct the widget but not make it visible (that's
#  for the client to do).
#
# OPTIONS:
#   -lines  - Number of lines of parameter editors.
#   -widget - Name of widget.
# PUBLIC METHODS:
#    update - Update the parameter menu list.
#    load   - Load the editor from a description of its contents.
#

::itcl::class parametersTabActions {
    public variable widget ""
    public variable lines 20

    #-------------------------------------------------------------------------------
    # Private Methods:
    # 

    ##
    # Get a list of the tree parameter names:
    #
    private method parameterList {} {
	set treeParameters [treeparameter -list]
	set result [list]

	foreach param $treeParameters {
	    lappend result [lindex $param 0]
	}
	
	return $result
    }

    ##
    # Load a specific parameter slot with data:
    # @param slot - the slot to load.
    # @param paath - The tree parameter path.
    #
    private method loadSlot {slot path} {

	set paramInfo [treeparameter -list $path]

	# For now if there are duplicates, just take the first

	if {[llength $paramInfo] > 0} {
	    set paramInfo [lindex $paramInfo 0]
	    $widget load $slot [lindex $paramInfo 0] \
		[lindex $paramInfo 2]                 \
		[lindex $paramInfo 3]                 \
		[lindex $paramInfo 5]
	}
    } 



    ##
    # list the spectra that depend on a parameter.
    # @param parameter - The parameter to check on.
    # @return list
    # @retval possibly empty list of dependent spectra.
    #
    private method listDependentSpectra {parameter} {
	set result [list]
	foreach spectrum [spectrum -list] {
	    set paramLists [lindex $spectrum 3]
	    foreach paramList $paramLists {
		if {[lsearch -exact $paramList $parameter] != -1} {
		    lappend result [lindex $spectrum 1]
		    break
		}
	    }
	}
	return $result
    }



    #-----------------------------------------------------------------------------
    #  Dialogs:

    ## 
    # Method to prompt for spectrum changes:
    # @param spectra list of spectra to prompt for.
    # @return bool
    # @retval true if change is approved false if not.
    # @note false is the default so that we can't accidently kill stuff as easily.
    #
    private method promptChangeOk spectra {
	set spectra [join $spectra {, }]
	set message "The following spectra wil be erased and replaced: \n$spectra\n"
	append message "Do you wish to continue?"

	set answer [tk_messageBox -type yesno -default no \
			-icon warning -message $message -parent $widget -title {Confirm Change}]
	
	return [expr $answer eq "no"]

    }

    ##
    # Method to indicate there are no affected spectra:
    #
    private method notifyNoMatches {} {
	tk_messageBox -type ok -icon info -message "No spectra use this parameter" -title {No Matches}

    }

    #-------------------------------------------------------------------------------
    #  Public interface:
    #

    ##
    # Construct the object
    #  @param args - a list of the option/values to use in construction time.
    #
    constructor {args} {
	eval configure $args;	# Set the widget name.
	if {$widget eq ""} {
	    error "The -widget option is mandatory for parametersTabActions"
	}

	treeParametersContainer $widget -number $lines -parameters [parameterList] \
	    -choosecmd [list $this loadCurrentEditor %N] \
	    -loadcmd   [list $this reloadEditor  %S]         \
	    -set       [list $this setParameter  %S]         \
	    -change    [list $this changeSpectra %S]

    }



    #----------------------------------------------------------------------------
    #  Internal callbacks.
    #  NOTE: itcl requires these to be public but they are not part of the class 
    #        interface.

    ##
    # Load the currently selected editor with a specific tree parameter
    # selected from the parameter pull right menu:
    #
    # @param path - the name of the parameter to load into the editor.
    #
    # @note - if the tree parameter does not exist, this is a no-op for now.
    #
    public method loadCurrentEditor path {

	loadSlot [$widget cget -current] $path

    }

    ##
    # Handle the load button
    # @param slot  - the slot whose load button was clicked.
    #
    #
    public method reloadEditor {slot} {
	set contents [$widget get $slot]
	set path     [lindex $contents 0]

	loadSlot $slot $path

    }

    ## 
    #  Handle the Set button.
    #
    # @param slot - the slot to modify.
    #
    public method setParameter {slot} {
	set contents [$widget get $slot]
	set path     [lindex $contents 0]

	# If there is no tree parameter by this name, we don't need
	# to do anything:

	set currentInfo [treeparameter -list $path]
	if {[llength $currentInfo] > 0} {
	    set currentInfo [lindex $currentInfo 0]

	    set low   [lindex $contents 1]
	    set hi    [lindex $contents 2]
	    set units [lindex $contents 3]
	    set bins  [lindex $currentInfo 1]

	    # Compute the increment _sigh_ about overdtermination and
	    # goddamned tree parameters.

	    set inc [expr {(1.0*$hi - $low)/$bins}]

	    treeparameter -set $path $bins $low $hi $inc $units
	    
	}
    }

    ##
    # Handles the change button:
    # - figures out which spectra depend on the parameter
    # - Prompts the user for go/nogo.
    # - On go, iterates through the spectra changing the appropriate axis
    #   limits/bins to matchthe tree parameter axis/bins.
    #
    # @param slot - The slot whose change button was clicked.
    # 
    public method changeSpectra {slot} {
	set contents [$widget get $slot]
	set path     [lindex $contents 0]

	set currentInfo [treeparameter -list $path]

	# The spectrum is set from the current tree parameter values:

	if {[llength $currentInfo] > 0} {
	    set currentInfo [lindex $currentInfo 0]
	    set bins [lindex $currentInfo 1]
	    set low  [lindex $currentInfo 2]
	    set hi   [lindex $currentInfo 3]

	    set spectra [listDependentSpectra $path]

	    # Get confirmation:

	    if {[llength $spectra] > 0} {
		if {[promptChangeOk $spectra]} {

		    modifySpectra $spectra $path $bins $low $hi
		}
	    } else {
		notifyNoMatches
	    }
	}

    }
}

