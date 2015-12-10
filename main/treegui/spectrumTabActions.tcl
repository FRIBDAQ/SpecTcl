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

package require spectrumContainer
package require guistate

package require autosave
package require restore

package provide spectrumTabActions 1.0


##
#  Class to supply the actions for the spectrum tab.
#

itcl::class spectrumTabActions {
    public variable widget;	# spectrumContainer widget option.

    private variable gateAddChain
    private variable gateDeleteChain
    private variable gateChangeChain
    private variable spectrumTypeNames

    private variable promptSpectrumOverwrite 0
   
    private variable gatesUpdateAfterId -1; # after id for scheduling a re-pop of the gate menu.
				 
    #-------------------------------------------------------------------------
    # Private utility methods

    ##
    # Schedule an update of the gate menu .5 seconds from now.  
    # addtional schedule prior the execution are no-op.
    #
    private method scheduleGateUpdate {} {
	if {$gatesUpdateAfterId == -1} {
	    set gatesUpdateAfterId [after 500 [itcl::code $this doScheduledGateUpdate]]
	}
    }
    ##
    # Perform a scheduled gate update operation:
    #
    private method doScheduledGateUpdate {} {
	set gatesUpdateAfterId -1
	LoadGateMenu
    }

    ## Given a number if it turns out the number is an integer,
    #  just return the integerized string.  If not trailing 0's
    #  are trimmed from the fp string (assuming not 1.0E10 notation).
    #
    #  @param value  - Value to modify
    #
    #  @return string 
    #  @retval value prettied up for display.
    #
    private proc niceDisplay value {
	set intValue [expr int($value)]
	if {$intValue == $value} {
	    return $intValue
	}

	
	return [string trimright $value 0]
    }
    
    ## Given a parameter name get the units:
    # @param name - parameter name
    # @return string 
    # @retval possibly empty string containing parameter units.
    #
    private method getUnits name {
	set def [parameter -list $name]
	if {[llength $def] > 0} {
	    set def [lindex $def 0]
	    set paramInfo [lindex $def 3]
	    return [lindex $paramInfo 2]

	}
	return {}
    }

    ##
    # Load the parameter names into the parameter menus.
    # 
    private method LoadParameters {} {
	set parameters [list]
	set time [time {
	foreach parameter [parameter -list] {
	    lappend parameters [lindex $parameter 0]
	}
	}]
	set time [time {$widget configure -parameters $parameters}]
    }

    ##
    # Get either selected or all spectra depending on the state of the all button.
    # @return list
    # @retval if all is checked a list of all spectrum names.
    # @retval if all is not checked a list of the names of spectr that are selected
    #         in the spectrum table.
    #
    private method getSelectedSpectra {} {
	if {[$widget cget -all]} {
	    set result [list]
	    foreach spectrum [spectrum -list] {
		lappend result [lindex $spectrum 1]
	    }
	    return $result
	} else {
	    return [$widget getSelection]
	}
    }
    ##
    # Determine if any of a list of items is null.
    # @param list - list of items
    # @return boolean
    # @return true - if at least one item is null.
    # @return false - if no items are null.
    #
    private method anyNulls list {
	foreach item $list {
	    if {$item eq ""} {
		return 1
	    }
	}
	return 0
    }
    ##
    # Generate a new spectrum name based on an existing one.
    # There is an assumption we will make... That already duplicated spectra
    # will be of the form name_integer.  There fore if a spectrum breask up into
    # a list of _ separated components with size > 1, and the last element is an integer
    # we should generate the spectra by incrementing the digits until we come up with one that
    # results in a spectrum name that is not yet used.  Otherwise just append _1 to the 
    # initial name...incrementing that until we get uniqueness.
    #
    #
    # @param baseName - The initial name of the spectrum.
    # @return string
    # @retval - a spectrum name  that is not yet in use.
    #
    private method generateUniqueSpectrumName baseName {
	
	# Figure out what the base part of the name is and the 
	# trailing integer should start with.
	#
	set baseList [split $baseName _]
	if {([llength $baseList] > 1) && ([string is integer [lindex $baseList end]])} {
	    set counter [lindex $baseList end]
	    set baseList [lrange $baseList 0 end-1]; #  lop off the last element.
	    incr counter;			     # and start hunting with the next integer.
	} else {
	    set counter 1
	}
	# Now hunt for a unique name by successively incrementing counter

	while 1 {
	    set candidateName [join [concat $baseList $counter] _]
	    set info [spectrum -list $candidateName]
	    if {[llength $info] == 0} {
		return $candidateName
	    } else {
		incr counter
	    }
	}
	
	
    }
    ##
    # True if the user says its ok to replace a spectrum:
    # @param name - the spectrum name.
    # @return bool
    # @retval true if the user accepts.
    #
    private method okToReplaceSpectrum name {

	# If prompting is turned off, it's always ok:

	if {$promptSpectrumOverwrite} {

	    if {[llength [spectrum -list $name]] > 0} {
		if {[tk_messageBox -default cancel -icon warning -parent $widget \
			 -title Overwite -type okcancel \
			 -message "$name already exists replace?"] eq "ok"} {
		    return 1
		} else {
		    return 0
		}
	    } else {
		# It's always ok to replace a nonexistent spectrum:
		
		return 1
	    }
	} else {
	    return 1
	}
    }
    # Duplicate a spectrum:
    # - Assign a  unique name that starts like the existing spectrum.
    # - Get the spectrum defintion.
    # - Create the new spectrum
    # - bind it to the display.
    # @param name - Name of the existing spectrum to duplicate.
    #
    private method duplicateSpectrum name {
	set newName [generateUniqueSpectrumName $name]
	set def     [spectrum -list $name]

	# bypass everything if there are no matching spectra.  This can happen if the
	# spectrum was deleted bu tthe display not updated.
	#
	if {[llength $def] > 0} {
	    set def [lindex $def 0]; # The actual definition.

	    set type     [lindex $def 2]
	    set param    [lindex $def 3]
	    set axes     [lindex $def 4]
	    set dataType [lindex $def 5]

	    spectrum $newName $type $param $axes $dataType
	    sbind $newName;	# Put it into Xamine.
	}
    }
    ##
    # Called to refresh the contents of the gate menu.
    #
    private method LoadGateMenu {} {
	set gates [list]
	foreach gate [gate -list] {
	    if {[lindex $gate 2] ne "F"} { # Only show non-false gates (issue 352).

		lappend gates [lindex $gate 0]
	    }
	}
	$widget configure -gates $gates
    }
    #--------------------------------------------------------------------------
    # Call back methods.  (private)
    #

    ##
    # Save the configuration
    # @param file   - name of the file to save it to.
    #
    private method SaveConfiguration {file} {
	set fd [open $file w]

	# MC Gui does not emit spectrum deletes....
	
	set prior $::guistate::writeDeletes
	set ::guistate::writeDeletes 0

	# write the definition file.

	writeAll $fd
	close $fd

	# Restore the write deletes flag back to what it was.

	set ::guistate::writeDeletes $prior


    }
    ##
    # Read the configuration the 'cumulate' sic checkbutton is used to determine
    # if the current defintions are wiped out first.
    # @param name   - filename from which the configuration is loaded.
    # @param defwidget - the defintionFile widget (has the -accumulate option we can query)
    #                 that triggered this.
    #
    private method ReadConfiguration {name defwidget} {
	set noclear [$defwidget cget -accumulate]

	# If noclear is not set we need to destroy the spectra as redefinition
	# is an error:

	if {!$noclear} {
	    set spectra [spectrum -list]
	    foreach spectrum $spectra {
		set sname [lindex $spectrum 1]
		spectrum -delete $sname
	    }
	}
	# Now we can read the file:
	#
	
	[Restore::getInstance] restore $name
	
	[autoSave::getInstance] failsafeSave

	sbind -all;		# Make spectra visible to Xamine.
	LoadSpectra [$widget cget -mask]
	
	
    }
    ##
    # Load the list of spectra from the current mask 
    # @param mask - glob pattern that determines the set of spectra to load.
    #
    private method LoadSpectra mask {
	set time [time {
	set spectra [spectrum -list -showgate $mask]
	}]
	set spectrumList [list]; # Build up the data here:

	# Pull each definition apart and add it to spectrumList.
	
	set time [time {
	foreach spectrum $spectra {
	    set name [lindex $spectrum 1]
	    set type [lindex $spectrum 2]
	    set parameters [lindex $spectrum 3]
	    set axes       [lindex $spectrum 4]
	    set dtype     [string range [lindex $spectrum 5] 0 0]
	    set gate       [lindex $spectrum 6]
	    
	    append type " $dtype"
	    
	    # Ungated true gate -> ""
	    if {$gate eq "-TRUE-" || $gate eq "-Ungated-"} {
		set gate ""
	    }

	    set xparam [lindex [lindex $parameters 0] 0]
	    set yparam [lindex [lindex $parameters 1] 0]

	    set xaxis [lindex $axes 0]
	    set xlow [niceDisplay [lindex $xaxis 0]]
	    set xhi  [niceDisplay [lindex $xaxis 1]]
	    set xbins [niceDisplay [lindex $xaxis 2]]

	    set yaxis [lindex $axes 1]
	    if {[llength $yaxis] > 0 && [is2DSpectrum $type] && ($type ne "g2")} {
		set ylow [niceDisplay [lindex $yaxis 0]]
		set yhi  [niceDisplay [lindex $yaxis 1]]
		set ybins [niceDisplay [lindex $yaxis 2]]
		
	    } else {
		set ylow ""
		set yhi  ""
		set ybins ""
	    }

	    # If there's a mapping from spectrum type to something more readable
	    #  in spectrumTypeNames use it instead of the type:

	    if {[array names spectrumTypeNames $type] ne ""} {
		set type $spectrumTypeNames($type)
	    }

	    lappend spectrumList [list $name $type              \
				      $xparam $xlow $xhi $xbins \
				      $yparam $ylow $yhi $ybins \
				      $gate]

	}}]
	set time [time {
	$widget configure -spectra $spectrumList
	}]
    }
    ##
    # Called in response to the button to clear spectra.
    # If the -all option is true we clear all of the spectra otherwise
    # only the spectra selected in the spectrum table are cleared.
    #
    private method ClearSpectra {} {
	if {[$widget cget -all]} {
	    clear -all
	} else {
	    ::treeutility::for_each clear [$widget getSelection]
	}
    }
    ##
    # Called in response to the button to delete spectra.
    #
    private method DeleteSpectra {} {
	if {[$widget cget -all]} {
	    spectrum -delete -all
	} else {
	    ::treeutility::for_each [list spectrum -delete] [$widget getSelection]
	}
	LoadSpectra [$widget cget -mask]
	[autoSave::getInstance]  failsafeSave
    }

    ##
    #  Called in response to the button to duplicate spectra.
    #  We're going to use the following private methods:
    #  getSelectedSpectra - Gets the list of spectra to operate on.
    #  duplicateSpectrum - Duplicates a single spetrum.
    #
    private method DupSpectra {} {

	::treeutility::for_each [itcl::code $this duplicateSpectrum] [getSelectedSpectra]
	LoadSpectra [$widget cget -mask]
	[autoSave::getInstance]  failsafeSave

    }

    ##
    # Called in response to the ungate button.  Ungates either the selected
    # or all spectra depending on the state of the all checkbutton.
    #
    private method UngateSpectra {} {
	set spectra [getSelectedSpectra]
	if {[llength $spectra] != 0} {
	    ungate {*}$spectra
	    LoadSpectra [$widget cget -mask]

	}
	[autoSave::getInstance]  failsafeSave

    }
    ## 
    #  Gate traces...these are separated to support
    #  chaining.
    #  @param name - the name of the gate affected.

    private method gateAdded name {
	set status [catch {
	scheduleGateUpdate
	if {$gateAddChain ne ""} {
	    uplevel #0 [list $gateAddChain $name]
	}} msg]
	if {$status} {
	    puts "gateAdded failed: $msg $name"
	}
    }
    private method gateDeleted name {
	set status [catch {
	scheduleGateUpdate
	if {$gateDeleteChain ne ""} {
	    uplevel #0 [list $gateDeleteChain $name]
	}} msg]
	if {$status} {
	    puts "gateDeleted failed: $msg $name"
	}
    }
    private method gateChanged name {
	set status [catch {
	scheduleGateUpdate
	if {$gateChangeChain ne ""} {
	    uplevel #0 [list $gateChangeChain $name]
	}} msg]
	if {$status} {
	    puts "gateChanged failed: $msg $name"
	}
    }
    ## 
    # For a given spectrum return the applied gate or an empty string if no gate is applied.
    #
    # @param name - spectrum name
    #
    # @return string
    # @retval possibly empty string of the applied gate.
    #
    private method AppliedGate name {
	set list [apply -list $name]
	if {[llength $list] != 0} {
	    set applied [lindex $list 0]
	    return [lindex [lindex $applied 1] 0]; # gate name.
	} else {
	    return ""
	}
    }
    ##
    # Given a spectrum name and a gate name, apply the gate the the spectrum.
    # If the gate name is an empty string, this is a no-op.
    #
    # @param specName - spectrum name.
    # @param gate - Gate name.
    #
    private method ApplyGate {specName gate} {
	if {[llength [gate -list $gate]] > 0} {
	    apply $gate $specName
	}
    }

    ## 
    # Whenever a gate is selected its full path is put in the entry below the menu:
    # @param name - full name of the gate.
    #
    private method Selectgate name {
	$widget configure -gate $name
	ApplyGates;		# Apply gates to selected spectra.
    }
    
    ##
    #  Apply gates to spectra.
    #
    private method ApplyGates {} {
	set spectra [getSelectedSpectra]
	set gate    [$widget cget -gate]
	if {[llength $spectra] != 0} {
	    apply $gate {*}$spectra
	}
	LoadSpectra [$widget cget -mask]
	[autoSave::getInstance]  failsafeSave

    }
    ##
    # Determines if a spectrum type is really a 2d spectrum from the poin of view
    # of the way the parameters are displayed in the tab:
    #
    # @param type  - spectrum type.
    #
    # @return boolean 
    # @retval true if a y parameter should be displayed/loaded.
    #
    private method is2DSpectrum type {
	set type [lindex $type 0]
	return [expr {$type in [list 2 S m2 gd]}]
    }

    ##
    # Select a spectrum and load it into the spectrum definition fields.
    #
    # @param name - The name of the spectrum.
    #
    private method SelectSpectrum name {
	set def [spectrum -list $name]

	# If the spectrum became undefined do nothing:

	if {[llength $def] > 0} {
	    set def      [lindex $def 0]
	    set name     [lindex $def 1]
	    set type     [lindex $def 2]
	    set params   [lindex $def 3]
	    set axes     [lindex $def 4]
	    set datatype [lindex $def 5]

	    set xParam [lindex $params 0]
	    set xAxis  [lindex $axes 0]
	    set xlow [lindex $xAxis 0]
	    set xhi  [lindex $xAxis 1]
	    set xbins [lindex $xAxis 2]


	    $widget configure -spectrumtype $type \
		-datatype $datatype               \
		-spectrumname $name               \
		-xparameter $xParam               \
		-xlow       [niceDisplay $xlow]   \
		-xhi        [niceDisplay $xhi]    \
		-xbins      [niceDisplay $xbins]

	    # Figure out units

	    $widget configure -xunits [getUnits $xParam]

	    

	    if {[llength $params] > 1 && [is2DSpectrum $type] && $type ne "gd"} {
		
		set yparam [lindex $params 1]
		set yaxis  [lindex $axes 1]
		set ylow   [lindex $yaxis 0]
		set yhi    [lindex $yaxis 1]
		set ybins  [lindex $yaxis 2]
		
		$widget configure -yparameter $yparam \
		    -ylow [niceDisplay $ylow] \
		    -yhi [niceDisplay $yhi] \
		    -ybins [niceDisplay $ybins] \
		    -yunits [getUnits $yparam] -ystate normal
	    } else {
		$widget configure -ystate disabled
	    }

	}
    }
    ##
    # Load a parameter into one of the axis widgets along with the suggested
    # values (if this is a tree parameter).
    # @param which - x or y - selects which of the  parameter widgets to load.
    # @param name           - Name of the parameter.
    #
    private method LoadParameter {which name} {
	#
	# If there's a tree parameter by that name use it:

        set tdef [treeparameter -list $name]
	if {[llength $tdef] > 0} {
	    set tdef [lindex $tdef 0]
	    set bins [lindex $tdef 1]
	    set lo   [lindex $tdef 2]
	    set hi   [lindex $tdef 3]
	    set units [lindex $tdef 5]
	    

	} else {
	    # Otherwise if there's a parameter by that name use what we can from it:
	    
	    set pdef [parameter -list $name]
	    if {[llength $pdef] > 0} {
		set pdef [lindex $pdef 0]
		set info [lindex $pdef 3]
		set bins [list]
		set lo   [lindex $info 0]
		set hi   [lindex $info 1]
		set units [lindex $info 2]

	    } else {
		# no such parameter so exit out:
		return
	    }
	}
	# If we got here the name, lo, hi, bins and units are set:

	$widget configure \
	    -${which}parameter $name \
	    -${which}low       $lo   \
	    -${which}hi        $hi   \
	    -${which}bins      $bins \
	    -${which}units     $units 


    }
    ##
    #  The spectrum type changed..figure out what the state of the y axis should be.
    #  only if it's 2, or S (stripchart) should we enable it:
    # 
    #  If 2d spectrum then we can enable byte data types other wise
    #  disable and, if the current datatype is byte set it to long.
    #
    private method ChangeSpectype {} {
	set type [$widget cget -spectrumtype]
	if { $type in [list 2 S ] } {
	    $widget configure -ystate normal -arraystate disabled
	} elseif {$type in [list s g2]} {
	    $widget configure -ystate disabled -arraystate disabled
	} else {
	    $widget configure -ystate disabled -arraystate normal
	}
	# Handle the radio button enables etc.

	if {$type eq 2} {
	    $widget enableDataType byte
	} else {
	    $widget disableDataType byte
	    if {[$widget cget -datatype] eq "byte"} {
		$widget configure -datatype long
	    }
	}
    }

    ## Invoked to create a spectrum.
    #
    private method CreateSpectrum {} {
	# If the spectrum exists prompt for redef:

	#
	#  We must have at least the following:
	# - spectrum name
	# - spectrum type
	# - Xaxis.   The rest depends on the type of spectrum.

	set type     [$widget cget -spectrumtype]
	set datatype [$widget cget -datatype]
	set name     [$widget cget -spectrumname]

	set xname    [$widget cget -xparameter]
	set xlow     [$widget cget -xlow]
	set xhi      [$widget cget -xhi]
	set xbins    [$widget cget -xbins]

	    
	
	# Do nothing if any of the above are empty:

	if {[anyNulls [list $type $datatype $name $xname $xlow $xhi $xbins]]} {
	    return
	}

	if {![string is double -strict  $xlow] ||
	    ![string is double -strict  $xhi]} {
	    ::treeutility::errorMessage \
		"Invalid X axis specification. Both low and high must be valid floats were: $xlow $xhi"
	    return
	}
	    


	# Bins must be integers:

	if {![string is integer $xbins]} {
	    ::treeutility::errorMessage "The number of bins on the x axis must be an integer was: $xbins"
	    return
	}
	if {$xbins < 0} {
	    ::treeutility::errorMessage "The number of bins on the x axis must be positive was $xbins"
	    return
	}

	# What happens next depends entirely on the spectrum type
	# Bitmask and 1d define essentially the same.
	# 2d and Stripchart need different defs.
	#

	switch -exact -- $type {
	    1 - b {
		#
		# 1d and bitmask spectra support the array checkbutton:
		#
		if {[$widget cget -array]} {
		    #  Get the names of the parameters and the corresponding spectrum names:

		    set parameterList [::treeutility::listArrayElements $xname ::treeutility::parameterList]
		    set spectrumList  [list]; # List of spectra to create
		    set existingSpectra [list];	# List of previously existing spectra:

		    foreach parameter $parameterList {
			set tail [lindex [split $parameter .] end]
			lappend spectrumList $name.$tail
			set currentInfo [spectrum -list $name.$tail]
			if {[llength $currentInfo] != 0} {
			    lappend existingSpectra [lindex [lindex $currentInfo 0] 1]
			}

		    }
		    #
		    # Be sure it's ok to re-define the existing spectra
		    #
		    if {([llength $existingSpectra] == 0) ||
			[::treeutility::okToReplaceSpectra $existingSpectra]} {
			foreach parameter $parameterList spectrum $spectrumList {
			    set gateName [AppliedGate $spectrum]
			    catch {spectrum -delete $spectrum}
			    spectrum $spectrum $type $parameter \
				[list [list $xlow $xhi $xbins]] $datatype
			    ApplyGate $spectrum $gateName
			    if {[catch {sbind $spectrum}]} {
				bindFailed $spectrumList $existingSpectra $gateName
				break;             # Rollback or kill all.
			    }
			}
		    }
		} else {
		    if {[okToReplaceSpectrum $name]} {
			set gateName [AppliedGate $name]
			set prior [spectrum -list $name];    # For rollback.
			catch {spectrum -delete $name}; # get rid of any prior spectrum.
			spectrum $name $type $xname [list [list $xlow $xhi $xbins]] $datatype
			ApplyGate $name $gateName
			if {[catch {sbind $name}]} {
			    bindFailed $name $prior $gateName
			}
		    }
		}
	    }
	    S {
		# Need a y parameter too:
		
		set yname [$widget cget -yparameter]

		if {($yname ne "") && [okToReplaceSpectrum $name]} {
		    set priorSpectra [spectrum -list $name]
		    set gateName [AppliedGate $name]
		    catch {spectrum -delete $name}
		    spectrum $name $type [list $xname $yname]  [list [list $xlow $xhi $xbins]] $datatype
		    ApplyGate $name $gateName
		    if {[catch {sbind $name}]} {
			bindFailed $name $priorSpectrum $gateName
		    }
		}
	    }
	    2 {
		# Need y parameter and axis definitions.

		set yname [$widget cget -yparameter]
		set ylow  [$widget cget -ylow]
		set yhi   [$widget cget -yhi]
		set ybins [$widget cget -ybins]

		if {![anyNulls [list $yname $ylow $yhi $ybins]] && [okToReplaceSpectrum $name]} {
		    if {![string is integer $ybins]} {
			::treeutility::errorMessage  "Bins on y axis must be an integer was: $ybins"
			return
		    }
		    if {$ybins < 0} {
			::treeutility::errorMessage  "Y bins must be a positive integer, was: $ybins"
			return
		    }
		    if {![string is double -strict  $ylow] ||
			![string is double -strict  $yhi]} {
			::treeutility::errorMessage \
			    "Invalid Y axis specification. Both low and high must be valid floats were: $ylow $yhi"
			return
		    }
		    
		    set gate [AppliedGate $name]
		    set prior [spectrum -list $name]
		    catch {spectrum -delete $name}
		    spectrum $name $type [list $xname $yname] \
			[list [list $xlow $xhi $xbins] [list $ylow $yhi $ybins]] $datatype
		    ApplyGate $name $gate
		    if {[catch {sbind $name}]} {
			bindFailed $name $prior $gate
		    }
		}
	    }
	    s - g1 {
		# Summary spectra, gamma 1d treat the parameters like an array instance:
		
		if {[okToReplaceSpectrum $name]} {
		    set parameterList [::treeutility::listArrayElements $xname ::treeutility::parameterList]
		    set gate [AppliedGate $name]
		    set prior [spectrum -list $name]
		    catch {spectrum -delete $name}
		    spectrum $name $type $parameterList [list [list $xlow $xhi $xbins]]
		    ApplyGate $name $gate
		    if {[catch {sbind $name}]} {
			bindFailed $name $prior $gate
		    }
		}
	    }
	    g2 {
		# Gamma 2 needs to make ay axis definitions that is identical to the X

		if {[okToReplaceSpectrum $name]} {

		    set parameterList [::treeutility::listArrayElements $xname ::treeutility::parameterList]
		    set gate [AppliedGate $name]
		    set prior [spectrum -list $name]
		    catch {spectrum -delete $name}
		    spectrum $name $type $parameterList \
			[list [list $xlow $xhi $xbins] [list $xlow $xhi $xbins]]
		    ApplyGate $name $gate
		    if {[catch {sbind $name}]} {
			bindFailed $name $prior $gate
		    }
		}

	    }
	    default {
		tk_messageBox -type ok -icon error -title {Can't make this spectrum} \
		    -parent $widget \
		    -message "The tree gui does not know how to create spectra of type: $type"
	    }
	}
		      
	LoadSpectra [$widget cget -mask]	  
	[autoSave::getInstance]  failsafeSave

    }
    ##
    # Called when the failsafe button has changed;
    # Get the state of the button and set the auotsave singleton accordingly.
    #
    #
    private method ChangeFailsafe {} {
	set state [$widget cget -makefailsafe]
	set autosave [autoSave::getInstance]

	if {$state} {
	    $autosave enableFailsafe
	} else {
	    $autosave disableFailsafe
	}
    }

    ## 
    # Called when a parameter name has been typed into in an axis
    # If the new entry field defines a parameter,  its low/high/bins/units are
    # loaded into the widget otherwise, ?'s are put in those field.s
    #
    # @param wid - spectrumAxis widget that changed.
    # @param name - New value of the parameter.
    #
    private method SetParameterInfo {wid {name ""}} {
	set info [treeparameter -list $name]


	if {[llength $info] == 0} {
	    $wid configure -low ? -high ? -bins ? -units ?
	} else {
	    set info [lindex $info 0]; # hopefully no wildcards in the name..if so get the first match.

	    $wid configure -low [lindex $info 2] \
		-high [lindex $info 3] \
		-bins [lindex $info 1] \
		-units [lindex $info 5]
	}
    }
    ##
    # bindFailed
    #    Called when the bind of a single spectrum failed.
    #    -  Notify the user of the problem.
    #    -  Delete the spectra created so far.
    #    -  Restore any spectra deleted prior to this.
    #
    # @param - names - names of spectra that need deleting.
    # @param - prior - Prior spectrum definitions that need creating.
    # @param - gate  - Gate applied to spectrum.
    private method bindFailed {names prior gate} {
	tk_messageBox					\
	    -icon error -title "Bind Failed" -type ok \
	    -message "Unable to bind a spectrum to shared memory - deleting the created spectra and restoring any deleted definitions"
	
	# delete the spectra- note not all names might have been created yet:
	
	foreach name $names {
	    catch {spectrum -delete $name}
	}
	# Restore any prior definitions.
	
	foreach spectrum $prior {
	    set name [lindex $spectrum 1]
	    set type [lindex $spectrum 2]
	    set params [lindex $spectrum 3]
	    set axes   [lindex $spectrum 4]
	    set dtype [lindex $spectrum 5]
	    
	    spectrum $name $type $params $axes $dtype
	    ApplyGate $name $gate
	}
    }
    #---------------------------------------------------------------------------
    # True public interface.  There are other public methods but they
    # require that exposure to be used as callbacks.
    #


    ##
    # Construct the object and view:
    # @paramargs - option/value pairs.. -widget is required.
    #
    constructor args {
	configure {*}$args

	array set spectrumTypeNames [list \
					 1 1d 2 2d s sum b bit \
					 S stp]

	if {$widget eq ""} {
	    error "The -widget option is mandatory"
	}
	set time [time {
	spectrumContainer $widget                           \
	    -ystate disabled -arraystate normal             \
	    -savecmd   [itcl::code $this SaveConfiguration %N]    \
	    -loadcmd   [itcl::code $this ReadConfiguration %N %W] \
	    -updatecmd [itcl::code $this LoadSpectra %M]          \
	    -clearcmd  [itcl::code $this ClearSpectra]            \
	    -deletecmd [itcl::code $this DeleteSpectra]           \
	    -dupcmd    [itcl::code $this DupSpectra]              \
	    -ungatecmd [itcl::code $this UngateSpectra]           \
	    -gateselectcmd [itcl::code $this Selectgate %N]       \
	    -applycmd      [itcl::code $this ApplyGates]          \
	    -selectcmd     [itcl::code $this SelectSpectrum %N]   \
	    -typechanged   [itcl::code $this ChangeSpectype]      \
	    -xparamselected [itcl::code $this LoadParameter x %N]    \
	    -yparamselected [itcl::code $this LoadParameter y %N] \
	    -createcmd      [itcl::code $this CreateSpectrum]     \
	    -failsafechanged [itcl::code $this ChangeFailsafe]    \
	    -xchanged        [itcl::code $this SetParameterInfo %W %T]  \
	    -ychanged        [itcl::code $this SetParameterInfo %W %T]}
		 ]


	set time [time LoadParameters]

	set time [time {LoadSpectra [$widget cget -mask]}]

	$widget disableDataType byte
	$widget configure -datatype long -spectrumtype 1

	# Load the gate menu and set it up to reload each time gates change in any way:

	set time [time LoadGateMenu]

	set gateAddChain    [gate -trace add    [itcl::code $this gateAdded]]
	set gateDeleteChain [gate -trace delete [itcl::code $this gateDeleted]]
	set gateChangeChain [gate -trace change [itcl::code $this gateChanged]]
	

	
    }
}