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

package provide spectrumTabActions 1.0


##
#  Class to supply the actions for the spectrum tab.
#

itcl::class spectrumTabActions {
    public variable widget;	# spectrumContainer widget option.
    #-------------------------------------------------------------------------
    # Private utility methods

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

    #--------------------------------------------------------------------------
    # Call back methods.  These are, by necesity public thought not really part of
    # the public interface.
    #

    ##
    # Save the configuration
    # @param file   - name of the file to save it to.
    #
    public method SaveConfiguration {file} {
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
    # @param widget - the defintionFile widget (has the -accumulate option we can query)
    #                 that triggered this.
    #
    public method ReadConfiguration {name widget} {
	set noclear [$widget cget -accumulate]

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
	uplevel #0 source $name
	
	
    }
    ##
    # Load the list of spectra from the current mask 
    # @param mask - glob pattern that determines the set of spectra to load.
    #
    public method LoadSpectra mask {
	set spectra [spectrum -list -showgate $mask]
	set spectrumList [list]; # Build up the data here:

	# Pull each definition apart and add it to spectrumList.
	
	foreach spectrum $spectra {
	    set name [lindex $spectrum 1]
	    set type [lindex $spectrum 2]
	    set parameters [lindex $spectrum 3]
	    set axes       [lindex $spectrum 4]
	    set gate       [lindex $spectrum 6]
	    
	    # Ungated true gate -> ""
	    if {$gate eq "-TRUE-" || $gate eq "-Ungated-"} {
		set gate ""
	    }

	    set xparam [lindex [lindex $parameters 0] 0]
	    set yparam [lindex [lindex $parameters 1] 0]

	    set xaxis [lindex $axes 0]
	    set xlow [lindex $xaxis 0]
	    set xhi  [lindex $xaxis 1]
	    set xbins [lindex $xaxis 2]

	    set yaxis [lindex $axes 1]
	    if {[llength $yaxis] > 0} {
		set ylow [lindex $yaxis 0]
		set yhi  [lindex $yaxis 1]
		set ybins [lindex $yaxis 2]
		
	    } else {
		set ylow ""
		set yhi  ""
		set ybins ""
	    }

	    lappend spectrumList [list $name $type              \
				      $xparam $xlow $xhi $xbins \
				      $yparam $ylow $yhi $ybins \
				      $gate]

	}
	$widget configure -spectra $spectrumList
    }
    ##
    # Called in response to the button to clear spectra.
    # If the -all option is true we clear all of the spectra otherwise
    # only the spectra selected in the spectrum table are cleared.
    #
    public method ClearSpectra {} {
	if {[$widget cget -all]} {
	    clear -all
	} else {
	    ::treeutility::for_each clear [$widget getSelection]
	}
    }
    ##
    # Called in response to the button to delete spectra.
    #
    public method DeleteSpectra {} {
	if {[$widget cget -all]} {
	    spectrum -delete -all
	} else {
	    ::treeutility::for_each [list spectrum -delete] [$widget getSelection]
	}
	LoadSpectra [$widget cget -mask]
    }
    # Duplicate a spectrum:
    # - Assign a  unique name that starts like the existing spectrum.
    # - Get the spectrum defintion.
    # - Create the new spectrum
    # - bind it to the display.
    # @param name - Name of the existing spectrum to duplicate.
    #
    public method duplicateSpectrum name {
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
	}
    }
    ##
    #  Called in response to the button to duplicate spectra.
    #  We're going to use the following private methods:
    #  getSelectedSpectra - Gets the list of spectra to operate on.
    #  duplicateSpectrum - Duplicates a single spetrum.
    #
    public method DupSpectra {} {

	::treeutility::for_each [list $this duplicateSpectrum] [getSelectedSpectra]
	LoadSpectra [$widget cget -mask]
    }

    ##
    # Called in response to the ungate button.  Ungates either the selected
    # or all spectra depending on the state of the all checkbutton.
    #
    public method UngateSpectra {} {
	set spectra [getSelectedSpectra]
	if {[llength $spectra] != 0} {
	    ungate {*}$spectra
	    LoadSpectra [$widget cget -mask]

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

	if {$widget eq ""} {
	    error "The -widget option is mandatory"
	}

	spectrumContainer $widget                           \
	    -savecmd   [list $this SaveConfiguration %N]    \
	    -loadcmd   [list $this ReadConfiguration %N %W] \
	    -updatecmd [list $this LoadSpectra %M]          \
	    -clearcmd  [list $this ClearSpectra]            \
	    -deletecmd [list $this DeleteSpectra]           \
	    -dupcmd    [list $this DupSpectra]              \
	    -ungatecmd [list $this UngateSpectra]

	LoadSpectra [$widget cget -mask]
    }
}