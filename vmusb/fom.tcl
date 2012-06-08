#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321




#  This file contains the proc fom.  the fom
#  proc defines a proc for producing the figure of merit
#  defined as (centroidn - centroidg)/(fwhmn+fwhmg).
#

# helper proc to find a named fit statistic.

proc fitstat {fit name} {
    set properties [lindex $fit 4]
    foreach item $properties {
	set pname   [lindex $item 0]
	set pval    [lindex $item 1]
	if {$pname eq $name} {
	    return $pval
	}
    }
    return ""
}


#  Requirements for an FOM is that the spectrum have
#  2 gaussian fits defined on it.
#  The fit with the larger centroid is assumed to be the
#  neutron fit.
# A result of -1 is returned if the spectrum does not have
# exactly 2 fits defined.

proc fom {spectrum} {
    set neutrons [fit list $spectrum-neutrons]
    if {[llength $neutrons] == 0} {
	return -1
    }
    set gammas [fit list $spectrum-gammas]
    if {[llength $gammas] == 0} {
	return -1
    }
    set neutrons [lindex $neutrons 0]
    set gammas   [lindex $gammas   0]

    # Extract the centroid/fwhm from the fits:

    set cent1 [fitstat $neutrons "centroid"]
    set cent2 [fitstat $gammas   "centroid"]
    set fwhm1 [fitstat $neutrons "sigma"]
    set fwhm2 [fitstat $gammas   "sigma"]


    set fwhm1 [expr 2.354*$fwhm1];	# Sigma -> fwhm
    set fwhm2 [expr 2.354*$fwhm2]



    return [expr abs(($cent1 - $cent2)/($fwhm1 + $fwhm2))]

}

# canGuiFOM
#    Returns an error message with suggested corrective action
#    if the conditions are not all there for computing a FOM
#    from the gui.
#    conditions are that the spetrum its
#    name-gammas 
#    name-neutrons must exist
#
proc canGuiFOM name {
    set neutrons [fit list $name-neutrons]
    if {[llength $neutrons] == 0} {
	error "Fit of neutrons peak for $name has not been created"
    }
    set gammas [fit list $name-gammas]
    if {[llength $gammas] == 0} {
	error "Fit of gamma peak for $name has not been created"
    }
}

#
#  ShowFOM   - show the figure of merit for specific spectrum to
#  a popup:
#
set fomboxId 0
proc ShowFOM spectrum {
    global fomboxId

    #  If the FOM cannot be created figure out why and error that:
    #
    if {[catch {canGuiFOM $spectrum} reason] } {
	set message "Unable to do Figure of merit computation: $reason"
    } else {

	fit update
	set value [fom $spectrum]
	set value [format %0.2f $value]
	
	if {$value != -1} {
	    set message "Figure of merit for $spectrum is $value"
	} else {
	    set message "Figure of merit for $spectrum is not defined"
	}
    }
    set top [toplevel .fom$fomboxId]
    incr fomboxId

    message $top.msg -text $message
    button  $top.dismiss -text Dismiss -command [list destroy $top]

    pack $top.msg $top.dismiss
}

# ShowFOMAll - Show the figure of merit for all spectra for which
#              it is defined.
#
proc ShowFOMAll {} {
    global fomboxId
    fit update
    set top [toplevel .foms$fomboxId]
    incr fomboxId

    listbox   $top.foms                    \
	-yscrollcommand [list $top.sb set] \
	-width 28
    scrollbar $top.sb     \
	-orient vertical  \
	-command [list $top.foms yview]
    button    $top.dismiss -text Dismiss -command [list destroy $top]
    button    $top.file    -text Save... -command [list saveFoms $top.foms]

    grid $top.foms -row 0   -column 0 -sticky nsew
    grid $top.sb   -row 0   -column 1 -sticky nsw
    grid $top.dismiss  $top.file

    foreach spectrum [spectrum -list] {
	set name [lindex $spectrum 1]
	if {![catch {canGuiFOM $name}]} {
	    set value [fom $name]
	    if {$value != -1} {
		set line [format "%-20s %5.2f" $name $value]
		$top.foms insert end $line
	    }
	}
    }

}
# saveFoms  - save the figure of merit calculations in the listbox
#             provided to file
#
proc saveFoms widget {
    global RunTitle
    global RunNumber

    set file [tk_getSaveFile -title "FOM File"                \
		  -filetypes {{{Text files} .txt}             \
				  {{All Files} *}}]
    if {$file eq ""} {
	return
    }

    set fd [open $file w]
    puts $fd "Figure of merit listing for run $RunNumber '$RunTitle'"
    puts $fd "Time: [clock format [clock seconds]]"
    puts $fd

    set data [$widget get 0 end]
    foreach fom $data {
	puts $fd $fom
    }
    close $fd

}