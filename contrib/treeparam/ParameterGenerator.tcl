# ParameterGenerator.tcl
# Author: D. Bazin
# Date: July 2001 - Modified September 2002

proc SetupParameterGenerator {parent} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global topp
#	UpdateTreeParameters
	CreateParameterGenerator $parent
#	set wname $topp.generate.parameter
#	GenerateTreePseudoMenu $wname GeneratePseudoParameter
}

proc CreateParameterGenerator {parent} {
	global topp
	set topp [tabnotebook_page $parent Parameters]
	
	CreateParameterManipulator $parent 1
	CreateParameterManipulator $parent 2
	CreateParameterManipulator $parent 3
	CreateParameterManipulator $parent 4
	CreateParameterManipulator $parent 5
		
	set gen $topp.generate
	set gencolor lightyellow
	frame $gen -borderwidth 2 -relief groove -background $gencolor
#		menubutton $gen.parameter -text "Generate" -background $gencolor
#		entry $gen.name -width 30 -textvariable newParameter -background $gencolor
#		label $gen.coming -text "Coming soon!" -background $gencolor
#		pack $gen.parameter $gen.name $gen.coming -side left
	pack $gen -expand 1 -fill both

	pack $topp -expand 1 -fill both
}

proc CreateParameterManipulator {parent id} {
	global topp parameter
	set mod $topp.modify$id
	set modcolor lightblue
	frame $mod -borderwidth 2 -relief groove -background $modcolor
		menubutton $mod.parameter -width 15 -text Parameter -background $modcolor
		GenerateTreeMenu $mod.parameter "MenuLoadParameter $id"
		label $mod.labelbits -text Bits  -background $modcolor
		label $mod.labelstart -text Start -background $modcolor
		label $mod.labelstop -text Stop -background $modcolor
		label $mod.labelinc -text Increment -background $modcolor
		label $mod.labelunit -text Unit -background $modcolor
		entry $mod.labelparameter -textvariable parameter(Name$id) -background $modcolor
		label $mod.bits -textvariable parameter(Bits$id) -background $modcolor
		entry $mod.start -width 8 -textvariable parameter(Start$id) -background $modcolor
		entry $mod.stop -width 8 -textvariable parameter(Stop$id) -background $modcolor
		entry $mod.inc -width 8 -textvariable parameter(Inc$id) -background $modcolor
		entry $mod.unit -width 6 -textvariable parameter(Unit$id) -background $modcolor
		button $mod.set -text Set -command "SetParameter $id" -background $modcolor
		button $mod.load -text Load -command "LoadParameter $id" -background $modcolor
		grid $mod.parameter $mod.labelbits $mod.labelstart $mod.labelstop $mod.labelinc $mod.labelunit $mod.load -sticky news
		grid $mod.labelparameter $mod.bits $mod.start $mod.stop $mod.inc $mod.unit $mod.set -sticky news
		grid columnconfigure $mod {0 1 2 3 4 5 6} -weight 1
	pack $mod -expand 1 -fill x
	trace variable parameter(Start$id) w "SetParameterStopInc $id"
	trace variable parameter(Stop$id) w "SetParameterStopInc $id"
	trace variable parameter(Inc$id) w "SetParameterStopInc $id"
}

proc SetParameterStopInc {id name1 name2 op} {
	global topp parameter
	if {[string equal [focus -displayof $topp] $topp.modify$id.stop]} {
		set parameter(Inc$id) [expr ($parameter(Stop$id) - $parameter(Start$id)) / (pow(2, $parameter(Bits$id)) - 1)]
	} elseif {[string equal [focus -displayof $topp] $topp.modify$id.start]} {
		set parameter(Inc$id) [expr ($parameter(Stop$id) - $parameter(Start$id)) / (pow(2, $parameter(Bits$id)) - 1)]
	} elseif {[string equal [focus -displayof $topp] $topp.modify$id.inc]} {
		set parameter(Stop$id) [expr $parameter(Start$id) + $parameter(Inc$id) * (pow(2, $parameter(Bits$id)) - 1)]
	}
}

proc SetParameter {id} {
	global parameter
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global spectrumInfoX spectrumInfoY spectrumGate
	global server
	SendMessage \
	"treeparameter -set $parameter(Name$id) $parameter(Start$id) $parameter(Stop$id) $parameter(Inc$id) $parameter(Unit$id)"
	GetResponse
	UpdateTreeParameter $parameter(Name$id)
	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
	set spectrumInfoY [GetParameterInfo $spectrumParameterY]
}

proc LoadParameter {id} {
	global parameter
	set par $parameter(Name$id)
	SendMessage "treeparameter -list $par"
	set p [lindex [GetResponse] 0]
	set parameter(Bits$id) [lindex $p 1]
	set parameter(Start$id) [lindex $p 2]
	set parameter(Stop$id) [lindex $p 3]
	set parameter(Inc$id) [lindex $p 4]
	set parameter(Unit$id) [lindex $p 5]
}

proc MenuLoadParameter {id par} {
	global parameter
	SendMessage "treeparameter -list $par"
	set p [lindex [GetResponse] 0]
	set parameter(Name$id) [lindex $p 0]
	set parameter(Bits$id) [lindex $p 1]
	set parameter(Start$id) [lindex $p 2]
	set parameter(Stop$id) [lindex $p 3]
	set parameter(Inc$id) [lindex $p 4]
	set parameter(Unit$id) [lindex $p 5]
}

proc GeneratePseudoParameter {parameter} {
	puts $parameter
}
