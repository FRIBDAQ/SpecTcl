# (C) Copyright Michigan State University 2019, All rights reserved 
# ParameterManipulator.tcl
# Author: D. Bazin
# Date: July 2001 - Modified September 2002
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
	set mod $topp.modify
	frame $mod -borderwidth 2 -relief groove -background $modcolor

	label $mod.labelname -text Name -background $modcolor
	label $mod.labelstart -text Low -background $modcolor
	label $mod.labelstop -text High -background $modcolor
#	label $mod.labelinc -text Increment -background $modcolor
	label $mod.labelunit -text Unit -background $modcolor
	checkbutton $mod.array -text "Array" -variable parameter(Array) -background $modcolor
	grid x $mod.labelname $mod.labelstart $mod.labelstop $mod.labelunit x $mod.array - -sticky news

	for {set i 1} {$i <= 20} {incr i} {
		InsertParameterManipulator $i
	}

	grid columnconfigure $mod {0 1 2 3 4 5 6 7} -weight 1
	pack $mod -expand 1 -fill both
		
	pack $topp -expand 1 -fill both
}

proc InsertParameterManipulator {id} {
	global topp parameter
	set modcolor lightblue
	set mod $topp.modify

	menubutton $mod.parameter$id -width 8 -text Parameter -background $modcolor
	GenerateTreeMenu $mod.parameter$id "MenuLoadParameter $id"
	entry $mod.labelparameter$id -textvariable parameter(Name$id) -background $modcolor
	entry $mod.start$id -width 8 -textvariable parameter(Start$id) -background $modcolor
	entry $mod.stop$id -width 8 -textvariable parameter(Stop$id) -background $modcolor
#	entry $mod.inc$id -width 8 -textvariable parameter(Inc$id) -background $modcolor
	entry $mod.unit$id -width 6 -textvariable parameter(Unit$id) -background $modcolor
	button $mod.set$id -text Set -command "SetParameter $id" -background $modcolor
	button $mod.load$id -text Load -command "LoadParameter $id" -background $modcolor
	button $mod.change$id -text "Change Spectra" -background lightgreen -command "ChangeSpectra $id" -width 10
	grid $mod.parameter$id $mod.labelparameter$id $mod.start$id $mod.stop$id $mod.unit$id \
	$mod.load$id $mod.set$id $mod.change$id -sticky news

#	trace variable parameter(Start$id) w "SetParameterStopInc $id"
#	trace variable parameter(Stop$id) w "SetParameterStopInc $id"
#	trace variable parameter(Inc$id) w "SetParameterStopInc $id"
}

#proc SetParameterStopInc {id name1 name2 op} {
#	global topp parameter
#	if {[string equal [focus -displayof $topp] $topp.modify$id.stop]} {
#		set parameter(Inc$id) [expr ($parameter(Stop$id) - $parameter(Start$id)) / ($parameter(Bins$id) - 1.0)]
#	} elseif {[string equal [focus -displayof $topp] $topp.modify$id.start]} {
#		set parameter(Inc$id) [expr ($parameter(Stop$id) - $parameter(Start$id)) / ($parameter(Bins$id) - 1.0)]
#	} elseif {[string equal [focus -displayof $topp] $topp.modify$id.inc]} {
#		set parameter(Stop$id) [expr $parameter(Start$id) + $parameter(Inc$id) * ($parameter(Bins$id) - 1.0)]
#	} elseif {[string equal [focus -displayof $topp] $topp.modify$id.bins]} {
#		set parameter(Inc$id) [expr ($parameter(Stop$id) - $parameter(Start$id)) / ($parameter(Bins$id) - 1.0)]
#	}
#}

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
#	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
#	set spectrumInfoY [GetParameterInfo $spectrumParameterY]
	set spectrumLowX [GetParameterLow $spectrumParameterX]
	set spectrumHighX [GetParameterHigh $spectrumParameterX]
#	set spectrumBinsX [GetParameterBins $spectrumParameterX]
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

proc ChangeSpectra {id} {
	global parameter
	set theList [spectrum -list]
	set sList ""
	set nList ""
	foreach s $theList {
		set sName [lindex $s 1]
		set parList [lindex $s 3]
		if {$parameter(Array) && [IsParameterArray $id]} {
			foreach p [ListParameterArray $id] {
				if {[lsearch $parList $p] != -1} {
					lappend sList $sName
					lappend nList [lsearch $parList $p]
					break
				}
			}
		} else {
			if {[lsearch $parList $parameter(Name$id)] != -1} {
				lappend sList $sName
				lappend nList [lsearch $parList $parameter(Name$id)]
			}
		}
	}
	set response [tk_messageBox -icon warning -type yesno -parent .gui\
	-message "The following spectra will be erased and replaced:\n$sList\nDo you wish to continue?"]
	if {[string equal $response no]} {
		return
	}
	SetParameter $id
	set index 0
	foreach s $sList {
		set ss [spectrum -list $s]
		set sType [lindex $ss 2]
		set sParam [lindex $ss 3]
		set sReso [lindex $ss 4]
		set sData [lindex $ss 5]
		set sNew ""
		for {set i 0} {$i < [llength $sReso]} {incr i} {
			if {$i == [lindex $nList $index]} {
				set bins [lindex [lindex $sReso $i] 2]
				lappend sNew [list $parameter(Start$id) $parameter(Stop$id) $bins]
			} else {
				lappend sNew [lindex $sReso $i]
			}
		}
		spectrum -delete $s
		spectrum $s $sType $sParam $sNew $sData
		incr index
	}
	UpdateSpectrumList
	sbind -all
	Modified
}

proc LoadParameter {id} {
	global parameter
	set par $parameter(Name$id)
	set p [lindex [treeparameter -list $par] 0]
#	set parameter(Bins$id) [lindex $p 1]
	set parameter(Start$id) [lindex $p 2]
	set parameter(Stop$id) [lindex $p 3]
#	set parameter(Inc$id) [lindex $p 4]
	set parameter(Unit$id) [lindex $p 5]
}

proc MenuLoadParameter {id par} {
	global parameter
	set p [lindex [treeparameter -list $par] 0]
	set parameter(Name$id) [lindex $p 0]
#	set parameter(Bins$id) [lindex $p 1]
	set parameter(Start$id) [lindex $p 2]
	set parameter(Stop$id) [lindex $p 3]
#	set parameter(Inc$id) [lindex $p 4]
	set parameter(Unit$id) [lindex $p 5]
}

#proc GeneratePseudoParameter {parameter} {
#	puts $parameter
#}
