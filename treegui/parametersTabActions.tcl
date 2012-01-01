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
package require treeUtilities

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

    private variable changeableSpectra [list 1 2 g1 g2 s m2 gd b X gs]
    #-------------------------------------------------------------------------------
    # Private Methods:
    # 


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
	    set specType   [lindex $spectrum 2]
	    foreach paramList $paramLists {
		if {([lsearch -exact $paramList $parameter] != -1) &&
		($specType in $changeableSpectra) } {
		    lappend result [lindex $spectrum 1]
		    break
		}
	    }
	}
	return $result
    }
    ##
    # Given an axis specification, return  a new specification with the specified axis modified.
    #
    # @param axes - axis specification list.
    # @param which - Index of axis to modify.
    # @param low   - New low value.
    # @param hi    - New high values.
    #
    # @return list
    # @retval modified axis list.
    #
    private method changeAxisLimits {axes which low hi} {
	set axis [lindex $axes $which]
	set axis [lreplace $axis 0 1 $low $hi]
	set axes [lreplace $axes $which $which $axis]

	return $axes

    }

    ##
    #  Modify spectra that depend on a parameter to match the new parameter specification.
    #  what is changed is the parameter range not the current number of bins.
    #  Which axis is affected depends on the type of the spectrum as follows:
    #  - 1, 2 - The axis on which the parameter lives is the axis modified.
    #  - g1, g2, s - All axes are modified for these spectra.
    #  - m2 - Even , gs matches modify x and odd matches modify y...both axes may be modified.
    #  - gd - Matches in the first list modify x matches in the second list modify y.
    #         both axes may be modified.
    #  - b - low and high are determined by log2 of low/high values..any log <= 0 => 0.
    #  - S - strip chart spectra are never modified.
    #
    # @param spectra   - the set of spectra that might be modified.
    # @param parameter - The parameter whose definition changed.
    # @param low       - New low limit on the parameter.
    # @param hi        - New high limit on the parameter.
    #
    # @note The bin count comes from the current axis specification.

    private method modifySpectra {spectra parameter low hi} {

	foreach spectrum $spectra {
	    set definition [lindex [spectrum -list $spectrum] 0]
	    set name [lindex $definition 1]
	    set type [lindex $definition 2]
	    set parameters [lindex $definition 3]
	    set axes       [lindex $definition 4]
	    set dataType   [lindex $definition 5]

	    # All spectra have at least one parameter list and an axis specification:

	    set xParams [lindex $parameters 0]
	    set xAxis   [lindex $axes       0]

	    # What happens next is type dependent:

	    switch -exact -- $type {
		1 - 2 {
		    set which [lsearch -exact $parameters $parameter]

		    # require a match...then change the specified axis.

		    if {$which >= 0} {
			set axes [changeAxisLimits $axes $which $low $hi]
		    }
		}
		g1 - g2 - s - gs {
		    # We would not be here if there wasn't a match.
		    #
		    for {set i 0} {$i < [llength $axes]} {incr i} {
			set axes [changeAxisLimits $axes $i $low $hi]
		    }
		}
		m2 {
		    set matchList [lsearch -exact -all  $parameters $parameter]
		    
		    array set matchArray [list]

		    # Build an array of elements whose indices are the axis numbers to modify.

		    foreach index $matchList {
			if {$index % 2} {
			    set matchArray(1) 1; # Odd parameter match.
			} else {
			    set matchArray(0) 1; # Even parameter match.
			}
		    }
		    # Modify the axes.

		    foreach axisNum [array names matchArray] {
			set axes [changeAxisLimits $axes $axisNum $low $hi]
		    }
		}
		gd {
		    if {[lsearch -exact $xParams $parameter] != -1}  {
			set axes [changeAxisLimits $axes 0 $low $hi]
		    }
		    if {[lsearch -exact [lindex $parameters 1] $parameter] != -1} {
			set axes [changeAxisLimits $axes 1 $low $hi]
		    }

		}
		b {
		    if {$low <= 0} {
			set l 0
		    } else {
			set l [expr {int(log($low)/log(2))}]; # change of base property to get log2
		    }
		    if {$hi <= 0} {
			set h 0
		    } else {
			set h [expr {int(log($hi)/log(2))}]
		    }
		    set axes [changeAxisLimits $axes 0 $l $h]
		}
	    }
	    

	    # Kill off the old spectrum, define the new and 
	    # sbind it to the display

	    spectrum -delete $name
	    spectrum $name $type $parameters $axes $dataType
	    sbind $name

	}
    }
    ##
    # List the parameters that match a parameter array given a sample parameter in the array.
    # We require that the suffixes on the parameter be numeric.
    # @param sampleName - An element of the array.
    # @return list
    # @retval Parameters in the array...if this is empty, sampleName was not an array element.
    #
    private method listArrayElements sampleName {

	return [::treeutility::listArrayElements $sampleName treeutility::parameterList]

    }

    ##
    # Get the parameters list affected if an array checkbox might expand the list:
    # @param path - the path of the parameter we are possibly expanding.
    # @return list
    # @retval actual set of affected parameters.
    # @note the -array configuration option of the editor widget determines
    #       whether or not we can expand the list.
    # @note If array is checked but the parameter is not an array element the parameter
    #       itself is returned.
    #
    private method getTargetParameters path {
	    # 
	    #  If the array button is checked we need to do
	    #  This for a bunch of parameters potentially
	    #

	    if {[$widget cget -array]} {
		set parameters [listArrayElements $path]

		# If the list is empty, then treat this as if the
		# parameter is not an array and it's as if the array checkbox was
		# not selected:

		if {[llength $parameters] == 0} {
		    set parameters [list $path]
		}
	    } else {
		set parameters [list $path]
	    }
	    return $parameters
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
	return [::treeutility::okToReplaceSpectra $spectra]
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


	treeParametersContainer $widget -number $lines -parameters [treeutility::parameterList] \
	    -choosecmd [list $this loadCurrentEditor %N] \
	    -loadcmd   [list $this reloadEditor  %S]         \
	    -set       [list $this setParameter  %S]         \
	    -change    [list $this changeSpectra %S]        \
	    -namechanged [list $this nameChanged %S]

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

	    # Enumerate the paths to set.  If the
	    # array checkbutton is checked we must treat this as
	    # an array.

	    set parameters [getTargetParameters $path]

	    
	    foreach parameter $parameters {
		treeparameter -set $parameter $bins $low $hi $inc $units
	    }
	    
	}
	[autoSave::getInstance] failsafeSave

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

	    # Potentially expand the parameter list:

	    set parameters [getTargetParameters $path]

	    # We need to figure out
	    # - The set of spectra that depend on each parameter in the list.
	    # - The merged set of spectra we'll modify.
	    # This will be done by creating an array indexed by spectrum to be
	    # modified with a list of parameters that cause the spectrum to be modified
	    # as the value:
	    #
	    array set spectraModified [list]
	    foreach parameter $parameters {
		set spectra [listDependentSpectra $parameter]
		foreach spectrum $spectra {
		    lappend spectraModified($spectrum) $parameter
		}
	    }
	    # The list of specta modified is therefore:

	    set spectra [array names spectraModified]

	    # Get confirmation:

	    if {[llength $spectra] > 0} {
		if {[promptChangeOk $spectra]} {

		    foreach spectrum [array names spectraModified] {
			foreach parameter $spectraModified($spectrum) {
			    modifySpectra $spectrum $parameter $low $hi
			}
			[autoSave::getInstance] failsafeSave; # Only if we modify spectra.
		    }
		}
	    } else {
		notifyNoMatches
	    }

	}

    }
    ##
    # Invoked if the name of a parameter widget changed.
    # - fetch the name.
    # - If the name corresponds to a treeparameter name load the editor.
    # - IF not put ? in the entries that contain the parameter information.
    #
    # @param slot - slot that changed.
    #
    public method nameChanged slot {
	set name [lindex  [$widget get $slot] 0]
	set paramInfo [treeparameter -list $name]
	if {[llength $paramInfo] > 0} {
	    loadSlot $slot $name
	} else {
	    $widget load $slot $name ? ? ?
	}


    }
}

