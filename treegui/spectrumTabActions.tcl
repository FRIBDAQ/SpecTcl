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
	    if {$gate eq "-TRUE-"} {
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
	    foreach name [$widget getSelection] {
		clear $name
	    }
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

	spectrumContainer $widget \
	    -savecmd   [list $this SaveConfiguration %N] \
	    -loadcmd   [list $this ReadConfiguration %N %W] \
	    -updatecmd [list $this LoadSpectra %M]  \
	    -clearcmd  [list $this ClearSpectra]

	LoadSpectra [$widget cget -mask]
    }
}