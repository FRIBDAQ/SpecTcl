# ParameterGenerator.tcl
# Author: D. Bazin
# Date: July 2001

source TreeParameter.tcl

proc SetupParameterGenerator {} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global topp
	UpdateTreeParameters
	CreateParameterGenerator
	set wname $topp.modify.parameter
	GenerateTreeMenu $wname LoadParameter
	set wname $topp.generate.parameter
	GenerateTreePseudoMenu $wname GeneratePseudoParameter
}

proc CreateParameterGenerator {} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global theParameter parameterBits parameterStart parameterStop parameterInc parameterUnit
	global topp
	if {[winfo exists .theTop]} {
		set topp .theTop.parametergenerator
		frame $topp -borderwidth 8 -relief groove
	} else {
		set topp .parametergenerator
		toplevel $topp
		wm title $topp "Parameter Generator"
	}
	
	set mod $topp.modify
	set modcolor lightblue
	frame $mod -borderwidth 4 -relief groove -background $modcolor
		menubutton $mod.parameter -width 15 -text "Parameter" -background $modcolor
		label $mod.labelbits -text "Bits"  -background $modcolor
		label $mod.labelstart -text "Start" -background $modcolor
		label $mod.labelstop -text "Stop" -background $modcolor
		label $mod.labelinc -text "Increment" -background $modcolor
		label $mod.labelunit -text "Unit" -background $modcolor
		label $mod.labelparameter -textvariable theParameter -background $modcolor
		label $mod.bits -textvariable parameterBits -background $modcolor
		entry $mod.start -width 8 -textvariable parameterStart -background $modcolor
		entry $mod.stop -width 8 -textvariable parameterStop -background $modcolor
		entry $mod.inc -width 8 -textvariable parameterInc -background $modcolor
		entry $mod.unit -width 6 -textvariable parameterUnit -background $modcolor
		button $mod.set -text "Set" -command {SetParameter $theParameter} -background $modcolor
		button $mod.load -text "Load" -command {LoadParameter $theParameter} -background $modcolor
		grid $mod.labelparameter $mod.bits $mod.start $mod.stop $mod.inc $mod.unit $mod.set -sticky news
		grid $mod.parameter $mod.labelbits $mod.labelstart $mod.labelstop $mod.labelinc $mod.labelunit $mod.load -sticky news
	pack $mod -expand 1 -fill both
	trace variable parameterStart w SetParameterStopInc
	trace variable parameterStop w SetParameterStopInc
	trace variable parameterInc w SetParameterStopInc
	
	set gen $topp.generate
	set gencolor lightyellow
	frame $gen -borderwidth 4 -relief groove -background $gencolor
		menubutton $gen.parameter -text "Generate" -background $gencolor
		entry $gen.name -width 30 -textvariable newParameter -background $gencolor
		label $gen.coming -text "Coming soon!" -background $gencolor
		pack $gen.parameter $gen.name $gen.coming -side left
	pack $gen -expand 1 -fill both

	if {[winfo exists .theTop]} {
		pack $topp -expand 1 -fill both
	}
}

proc SetParameterStopInc {name1 name2 op} {
	global theParameter parameterBits parameterStart parameterStop parameterInc parameterUnit
	if {[string match $name1 parameterStop] && [string match $parameterStop ""] == 0} {
		set parameterInc [expr ($parameterStop - $parameterStart) / (pow(2, $parameterBits) - 1)]
	} elseif {[string match $name1 parameterStart] && [string match $parameterStart ""] == 0} {
		set parameterInc [expr ($parameterStop - $parameterStart) / (pow(2, $parameterBits) - 1)]
	} elseif {[string match $name1 parameterInc] && [string match $parameterInc ""] == 0} {
		set parameterStop [expr $parameterStart + $parameterInc * (pow(2, $parameterBits) - 1)]
	}
}

proc SetParameterStop {name1 name2 op} {
	global theParameter parameterBits parameterStart parameterStop parameterInc parameterUnit
	if {[string match $parameterInc ""] == 0} {
		set parameterStop [expr $parameterStart + $parameterInc * (pow(2, $parameterBits - 1)]
	}
}

proc SetParameter {parameter} {
	global theParameter parameterBits parameterStart parameterStop parameterInc parameterUnit
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global spectrumInfoX spectrumInfoY spectrumGate
	treeparameter -set $parameter $parameterStart $parameterStop $parameterInc $parameterUnit
	UpdateTreeParameters
	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
	set spectrumInfoY [GetParameterInfo $spectrumParameterY]
}

proc LoadParameter {parameter} {
	global theParameter parameterBits parameterStart parameterStop parameterInc parameterUnit
	set theParameter $parameter
	set theList [treeparameter -list]
	foreach p $theList {
		if {[string match [lindex $p 0] $parameter]} {
			set parameterBits [lindex $p 1]
			set parameterStart [lindex $p 2]
			set parameterStop [lindex $p 3]
			set parameterInc [lindex $p 4]
			set parameterUnit [lindex $p 5]
			return
		}
	}
}

proc GeneratePseudoParameter {parameter} {
	puts $parameter
}
