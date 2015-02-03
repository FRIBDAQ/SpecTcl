# (C) Copyright Michigan State University 2019, All rights reserved 
# ParameterManipulator.tcl
# Author: D. Bazin, R. Fox
# Date: July 2001 - see svn revision history for full modification history.
#
# Version 1.2 - November 2003

proc SetupParameterManipulator {parent} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global topp
	CreateParameterManipulator $parent
#	set wname $topp.generate.parameter
#	GenerateTreePseudoMenu $wname GeneratePseudoParameter
}

proc CreateParameterManipulator {parent} {
	global topp parameter
	set topp [tabnotebook_page $parent Parameters]

	set modcolor lightblue
	set parameter(select) 1
	set mod $topp.modify
	frame $mod -borderwidth 2 -relief groove -background $modcolor

	menubutton $mod.parameter -width 8 -text Parameter -background $modcolor
	GenerateTreeMenu $mod.parameter  SetSelectedParameter
	label $mod.labelname -text Name -background $modcolor
	label $mod.labelstart -text Low -background $modcolor
	label $mod.labelstop -text High -background $modcolor
	label $mod.labelunit -text Unit -background $modcolor
	checkbutton $mod.array -text "Array" -variable parameter(Array) -background $modcolor
	grid $mod.parameter $mod.labelname $mod.labelstart $mod.labelstop $mod.labelunit x $mod.array - -sticky news

	for {set i 1} {$i <= 20} {incr i} {
		InsertParameterManipulator $i
	}

	grid columnconfigure $mod {0 1 2 3 4 5 6 7} -weight 1
	pack $mod -expand 1 -fill both
		
	pack $topp -expand 1 -fill both
}

proc SetSelectedParameter {name} {
	global parameter
	set id $parameter(select)
	set parameter(Name$id) $name
}

proc InsertParameterManipulator {id} {
	global topp parameter
	set modcolor lightblue
	set mod $topp.modify

	radiobutton $mod.select$id -width 8 -text "" -variable parameter(select) -value $id -bg $modcolor
	entry $mod.labelparameter$id -textvariable parameter(Name$id) -background $modcolor
	entry $mod.start$id -width 8 -textvariable parameter(Start$id) -background $modcolor
	entry $mod.stop$id -width 8 -textvariable parameter(Stop$id) -background $modcolor
	entry $mod.unit$id -width 6 -textvariable parameter(Unit$id) -background $modcolor
	button $mod.set$id -text Set -command "SetParameter $id" -background $modcolor
	button $mod.load$id -text Load -command "LoadParameter $id" -background $modcolor
	button $mod.change$id -text "Change Spectra" -background lightgreen -command "ChangeSpectra $id" -width 10
	grid $mod.select$id $mod.labelparameter$id $mod.start$id $mod.stop$id $mod.unit$id \
	$mod.load$id $mod.set$id $mod.change$id -sticky news


}


proc SetParameter {id} {
	global parameter
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global spectrumInfoX spectrumInfoY spectrumGate
	global spectrumLowX spectrumHighX spectrumBinsX spectrumUnitX
	global spectrumLowY spectrumHighY spectrumBinsY spectrumUnitY
	if {$parameter(Array) && [IsParameterArray $id]} {
		foreach p [ListParameterArray $id] {
		    treeparameter -setunit $p $parameter(Unit$id)
		    treeparameter -setlimits $p $parameter(Start$id) $parameter(Stop$id)
		}
		UpdateTreeParameters
	} else {
		treeparameter -setunit $parameter(Name$id) $parameter(Unit$id)
		treeparameter -setlimits $parameter(Name$id) $parameter(Start$id) $parameter(Stop$id)
		UpdateTreeParameter $parameter(Name$id)
	}
	set spectrumLowX [GetParameterLow $spectrumParameterX]
	set spectrumHighX [GetParameterHigh $spectrumParameterX]
	set spectrumUnitX [GetParameterUnit $spectrumParameterX]
	set spectrumLowY [GetParameterLow $spectrumParameterY]
	set spectrumHighY [GetParameterHigh $spectrumParameterY]
	set spectrumBinsY [GetParameterBins $spectrumParameterY]
	set spectrumUnitY [GetParameterUnit $spectrumParameterY]
 	Modified
}

proc IsParameterArray {id} {
	global parameter
	set suffix [lindex [split $parameter(Name$id) .] end]
	return [string is integer $suffix]
}

proc ListParameterArray {id} {
	global parameter
	set theList [treeparameter -list]
	set suffix [lindex [split $parameter(Name$id) .] end]
	set prefix [string trimright $parameter(Name$id) .$suffix]
	append prefix *
	set pList ""
	foreach p $theList {
		set name [lindex $p 0]
		if {[string match $prefix $name]} {
			lappend pList $name
		}
	}
	return $pList
}
#
# changeAxisLimits
#   Given a spectrum axis definition (could be more than one axis def
#   modifes one of the axis definitions to have a new low/high value.
#
# Parameters:
#    axes     - Old axis definitions.
#    which    - Which axis to change.
#    low      - new low value
#    high     - new high value
# Returns:
#    new axis definitions.
#
proc changeAxisLimits {axes which low high} {
    set axis [lindex $axes $which]
    set axis [lreplace $axis 0 1 $low $high]
    set axes [lreplace $axes $which $which $axis]

    return $axes
}

#
#  Change the definition of a spectrum after a dependent parameter
#  has been modified.  What this should mean is that for every parameter
#  that has been modified (remember you can modify an array of parameters),
#  Spectra that depend on at least one of those parameters will be re-created
#  with axis specifications for the modified parameters modifed to reflect the
#  parameter's new low/high limits (same number of channels as before I'm presuming).
#
#  Parameters:
#    id    - The id of the parameter that has been modified.
#  Implicit inputs:
#    SpecTcl parameter list,
#    parameter(Array) is true if the array checkbox is lit indicating
#                     the base of the parameter should be treated as an
#                     array for which all parameters must be changed.
#
proc ChangeSpectra {id} {
    global parameter
    set theList [spectrum -list]
    set sList ""
    set nList ""
    # Create the list of parameters that will be modified:
    #
    
    if {$parameter(Array) && [IsParameterArray $id]} {
	set parameterList [ListParameterArray $id]
    } else {
	set parameterList $parameter(Name$id)
    }
    foreach s $theList {
	set sName [lindex $s 1]
	set parList [lindex $s 3]
	foreach p $parameterList {
	    if {[lsearch $parList $p] != -1} {
		lappend sList $sName
		break
	    }
	}
    }

    
    # 
    #  At this point, slist is the list of spectra that will be changed if the
    #  user confirms.
    
    
    set response [tk_messageBox -icon warning -type yesno -parent .gui\
		      -message "The following spectra will be erased and replaced:\n$sList\nDo you wish to continue?"]
    if {[string equal $response no]} {
	return
    }
    
    # Iterate through sList and modify each spectrum according to the type
    # and set of parameters that match.
    # There are several sorts of spectra:
    #   Spectra for which the parameter index indicates the axis index to modify
    #  (1-d, 2-d)
    #  Spectra that don't get modified at all:
    #    Currently only the strip chart.
    #  Spectra where if there is any parameter match, all axes are modified:
    #   g1, g2, s
    #  Spectra that have to be handled specially:
    #   m2, gd, b
    #

    foreach spectrum $sList {
	set definition [lindex [spectrum -list $spectrum] 0];
	set name       [lindex $definition 1]
	set type       [lindex $definition 2]
	set parameters [lindex $definition 3]
	set axes       [lindex $definition 4]
	set dataType   [lindex $definition 5]

	switch -exact --  $type {
	    1 - 2 {
		# The index of the parameter match defines which axis is modified.
                # both axes could conceivably change!
		foreach p $parameterList {
		    set which [lsearch -exact $parameters $p]
		    if {$which >= 0} {
			set parameterDef [lindex [treeparameter -list $p] 0]
			set low [lindex $parameterDef 2]
			set high [lindex $parameterDef 3]
			set axes [changeAxisLimits $axes $which $low $high]

		    }
		}
	    }
	    g1 - g2 - s {
		# For the first match, all axes are changed.
		# all axes will change, first match is ok because if its an array parameter set,
		# all parmeters will get the same limits
		#
		set axisCount [llength $axes]
		foreach p $parameterList {
		    set which [lsearch -exact $parameters $p]
		    if {$which >= 0} {
			set parameterDef [lindex [treeparameter -list $p] 0]
			set low          [lindex $parameterDef 2]
			set high         [lindex $parameterDef 3]

			# Change all axes:

			for {set i 0} {$i < $axisCount} {incr i} {
			    set axes [changeAxisLimits $axes $i $low $high]
			}
			break
		    }
		}
	    }
	    m2 {
		# The parameter list are x/y pairs.  Therefore
		# we go for all matches, even matches modify the x axis
		# odd matches the y.

		foreach p $parameterList {
		    set which [lsearch -exact $parameters $p]
		    if {$which >= 0} {
			set axis [expr {$which % 2}];   # 0 or 1...
			set parameterDef [lindex [treeparameter -list $p] 0]
			set low          [lindex $parameterDef 2]
			set high         [lindex $parameterDef 3]
			set axes         [changeAxisLimits $axes $axis $low $high]
		    }
		}
	    }
	    gd {
		# Gamma deluxe:  This has two lists of parameters.
		# matches in the first list change x.
		# matches in the second list change y.

		set xParameters [lindex $parameters 0]
		set yParameters [lindex $parameters 1]
		foreach p $parameterList {
		    set which [lsearch -exact $xParameters $p]
		    if {$which >= 0} {
			set parameterDef [lindex [treeparameter -list $p] 0]
			set low          [lindex $parameterDef 2]
			set high         [lindex $parameterDef 3]
			set axes         [changeAxisLimits $axes 0 $low $high]
		    }

		    set which [lsearch -exact $yParameters $p]
		    if {$which >= 0} {
			set parameterDef [lindex [treeparameter -list $p] 0]
			set low          [lindex $parameterDef 2]
			set high         [lindex $parameterDef 3]
			set axes         [changeAxisLimits $axes 1 $low $high]
		    }

		}
	    }
	    b {
		# Bitmask spectrum.  The low/high are determined by the
		# log2 of the low/high values.. e.g. 
		# a range of 4 - 32 gives a spectrum range of
		# 2-5   can't take the log of <= 0, those are mapped to 0.
		#
		# we use log2(x) = loge(x)/loge(2)
		# non integer weirdness is truncated on the low end and incremented on the high end
		#
		foreach p $parameterList {
		    set which [lsearch -exact $parameters $p]
		    if {$which == 0} {
			set parameterDef [lindex [treeparameter -list $p] 0]
			set low          [lindex $parameterDef 2]
			set high         [lindex $parameterDef 3]
			if {$low <= 0} {
			    set low 0
			} else {
			    set low [expr {int(log($low)/log(2))}]
			}
			if {$high <= 0} {
			    set high 0
			} else {
			    set high [expr {int(ceil(log($high)/log(2)))}]
			}
			set axes [changeAxisLimits $axes 0 $low $high]
			break
		    }
		}
	    }
	}
	# Set the new spectrum definition:

	spectrum -delete $name
	spectrum $name $type $parameters $axes $dataType

    }
    
    
    UpdateSpectrumList
    sbind -all
    Modified
}

proc LoadParameter {id} {
	global parameter
	set par $parameter(Name$id)
	set p [lindex [treeparameter -list $par] 0]
	set parameter(Start$id) [lindex $p 2]
	set parameter(Stop$id) [lindex $p 3]
	set parameter(Unit$id) [lindex $p 5]
}

proc MenuLoadParameter {id p1 p2 p3} {
	global parameter
	set p [lindex [treeparameter -list $parameter(Name$id)] 0]
	if {[llength $p] == 0} {
		set parameter(Start$id) ???
		set parameter(Stop$id) ???
		set parameter(Unit$id) ???
		return
	}
	set parameter(Start$id) [lindex $p 2]
	set parameter(Stop$id) [lindex $p 3]
	set parameter(Unit$id) [lindex $p 5]
}

