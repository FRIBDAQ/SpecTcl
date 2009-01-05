# (C) Copyright Michigan State University 2019, All rights reserved 
# SpectrumGenerator.tcl
# Tcl/Tk script to generate a menu driven spectrum creation
# Author: D. Bazin
# Date: Dec 2000 - Sept 2002
# Version 1.2 November 2003


# Changelog
# 4/29/05 -Timothy Hoagland - s04.thoagland@wittenberg.edu
# Changed the SortGate, Sort, Write, and GetDependencies functions to ensure that all
# gates are defined in an order that prevents gates being defined before gates it 
# depends on
#
#4/30/05 -Timothy Hoagland - s04.thoagland@wittenberg.edu
# Added the DependsOnDeleted function to ensure that any gates that rely on 
# a deleted function were not written to file when a save happens

proc SetupSpectrumGenerator {parent} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global definitionFile tops definitionDirectory enableFailsafe spectrumMask
	UpdateTreeParameters
	CreateSpectrumGenerator $parent
	set spectrumType 1
	SpectrumType1D
	set spectrumDatatype long
	set wname $tops.bottom.varx.varmenu
#	GenerateTreeMenu $wname SpectrumParameterXCommand
	GenerateTreeMenu $wname "set spectrumParameterX"
	set wname $tops.bottom.vary.varmenu
#	GenerateTreeMenu $wname SpectrumParameterYCommand
	GenerateTreeMenu $wname "set spectrumParameterY"
	set wname $tops.middle.gate.menu
	GenerateGateMenu $wname SpectrumGateCommand
	set definitionFile Unknown
	set definitionDirectory [pwd]
	set enableFailsafe 1
	set spectrumMask *
	UpdateSpectrumList
}

proc CreateSpectrumGenerator {parent} {
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global spectrumInfoX spectrumInfoY ActivityMonitor
	global spectrumLowX spectrumHighX spectrumBinsX spectrumUnitX
	global spectrumLowY spectrumHighY spectrumBinsY spectrumUnitY
	global definitionFile tops deleteAll clearAll enableFailsafe spectrumMask
	set tops [tabnotebook_page $parent Spectra]
	
	set options $tops.options
	set optionscolor lightblue
	frame $options

		set type $options.type
		frame $type -borderwidth 2 -relief groove -background $optionscolor
			label $type.label -text "Spectrum type" -background $optionscolor
			frame $type.n -background $optionscolor
			radiobutton $type.n.1d -text "1D" -variable spectrumType -value 1 -command SpectrumType1D -background $optionscolor
			radiobutton $type.n.2d -text "2D" -variable spectrumType -value 2 -command SpectrumType2D -background $optionscolor
			radiobutton $type.n.sum -text "Summary" -variable spectrumType -value s -command SpectrumTypeSummary -background $optionscolor
			radiobutton $type.n.bit -text "Bitmask" -variable spectrumType -value b -command SpectrumTypeBitmask -background $optionscolor
			radiobutton $type.n.1g -text "Gamma1D" -variable spectrumType -value g1 -command SpectrumType1G -background $optionscolor
			radiobutton $type.n.2g -text "Gamma2D" -variable spectrumType -value g2 -command SpectrumType2G -background $optionscolor
			radiobutton $type.n.gp -text "Gamma Deluxe" -variable spectrumType -value gd -command SpectrumTypeGP -background $optionscolor
                        radiobutton $type.n.sc -text "StripChart" -variable spectrumType -value S -command SpectrumTypeS -background $optionscolor
		
                	grid $type.n.1d $type.n.bit -sticky w
			grid $type.n.2d $type.n.1g -sticky w
			grid $type.n.sum $type.n.2g -sticky w
                        grid $type.n.sc $type.n.gp  -sticky w
			pack $type.label $type.n
		pack $type -expand 1 -fill both -side left
	
		set datatype $options.datatype
		frame $datatype -borderwidth 2 -relief groove -background $optionscolor
			label $datatype.label -text "Data type" -background $optionscolor
			frame $datatype.n -background $optionscolor
			radiobutton $datatype.n.byte -text "Byte (8 bits)" -variable spectrumDatatype -value byte -background $optionscolor
			radiobutton $datatype.n.word -text "Word (16 bits)" -variable spectrumDatatype -value word -background $optionscolor
			radiobutton $datatype.n.long -text "Long (32 bits)" -variable spectrumDatatype -value long -background $optionscolor
			grid $datatype.n.byte -sticky w
			grid $datatype.n.word -sticky w
			grid $datatype.n.long -sticky w
			pack $datatype.label $datatype.n
		pack $datatype -expand 1 -fill both -side left
		
		set control $options.control
		frame $control -borderwidth 2 -relief groove -background $optionscolor
			label $control.label -text "Definition file"  -background $optionscolor
			button $control.load -text "Load" -command Load -width 5 -background $optionscolor
			button $control.save -text "Save" -command Save -width 5 -background $optionscolor
			checkbutton $control.delete -text "Cumulate" -variable deleteAll -background $optionscolor
			checkbutton $control.failsafe -text "Failsafe" -variable enableFailsafe -background $optionscolor
			label $control.file -textvariable definitionFile  -background $optionscolor
			grid $control.label - -sticky news
			grid $control.file - -sticky news
			grid $control.load $control.save -sticky w
			grid $control.delete $control.failsafe -sticky w
		pack $control -expand 1 -fill both -side right

	pack $options -expand 1 -fill both
	
	set middle $tops.middle
	set middlecolor lightyellow
	frame $middle -background $middlecolor

		set command $middle.command
		frame $command -borderwidth 2 -relief groove -background $middlecolor
			label $command.label -text "Spectrum name" -background $middlecolor
			entry $command.entry -width 40 -textvariable spectrumName -background $middlecolor
			button $command.generate -text "Create/Replace" -command GenerateSpectrum -background $middlecolor
			button $command.clear -text "Clear" -command ClearSpectra -background $middlecolor
			checkbutton $command.generatearray -text "Array" -variable spectrumArray -background $middlecolor
			checkbutton $command.clearall -text "All" -variable clearAll -background $middlecolor
			button $command.delete -text "Delete" -command DeleteSpectra -background $middlecolor
			button $command.duplicate -text "Duplicate" -command DuplicateSpectra -background $middlecolor
			grid $command.label $command.generate $command.clear $command.delete -sticky news
			grid $command.entry $command.generatearray $command.clearall $command.duplicate -sticky news
		pack $command -expand 1 -fill both -side left

		set gate $middle.gate
		frame $gate -borderwidth 2 -relief groove -background $middlecolor
			entry $gate.label -width 10 -textvariable spectrumGate -background $middlecolor
			menubutton $gate.menu -text "Gate" -background $middlecolor
			button $gate.apply -text "Apply" -command ApplyGate -background $middlecolor
			button $gate.ungate -text "Ungate" -command UngateSpectra -background $middlecolor
			grid $gate.menu $gate.apply -sticky news
			grid $gate.label $gate.ungate -sticky news
		pack $gate -expand 1 -fill both -side right
		bind $gate.menu <ButtonPress-1> "GenerateGateMenu $gate.menu SpectrumGateCommand"

	pack $middle -expand 1 -fill both
	
	set bottom $tops.bottom
	set bottomcolor pink
	frame $bottom -background $bottomcolor
	
		set varx $bottom.varx
		frame $varx -borderwidth 2 -relief groove -background $bottomcolor
			entry $varx.varlabel -textvariable spectrumParameterX -background $bottomcolor
			menubutton $varx.varmenu -text "X Parameter" -background $bottomcolor
			label $varx.lowlabel -text Low -background $bottomcolor 
			entry $varx.low -textvariable spectrumLowX -background $bottomcolor -width 6
			label $varx.highlabel -text High -background $bottomcolor
			entry $varx.high -textvariable spectrumHighX -background $bottomcolor -width 6
			menubutton $varx.binsmenu -text Bins -background $bottomcolor 
			entry $varx.bins -textvariable spectrumBinsX -background $bottomcolor -width 4
			label $varx.unitlabel -text Unit -background $bottomcolor
			label $varx.unit -textvariable spectrumUnitX -background $bottomcolor -width 8
			grid $varx.varmenu $varx.lowlabel  $varx.highlabel $varx.binsmenu $varx.unitlabel -sticky news
			grid $varx.varlabel $varx.low $varx.high $varx.bins $varx.unit -sticky news
		pack $varx -expand 1 -fill both -side left
		
		set vary $bottom.vary
		frame $vary -borderwidth 2 -relief groove -background $bottomcolor
			entry $vary.varlabel -textvariable spectrumParameterY -background $bottomcolor
			menubutton $vary.varmenu -text "Y Parameter" -background $bottomcolor
			label $vary.lowlabel -text Low -background $bottomcolor
			entry $vary.low -textvariable spectrumLowY -background $bottomcolor -width 6
			label $vary.highlabel -text High -background $bottomcolor
			entry $vary.high -textvariable spectrumHighY -background $bottomcolor -width 6
			menubutton $vary.binsmenu -text Bins -background $bottomcolor
			entry $vary.bins -textvariable spectrumBinsY -background $bottomcolor -width 4
			label $vary.unitlabel -text Unit -background $bottomcolor
			label $vary.unit -textvariable spectrumUnitY -background $bottomcolor -width 8
			grid $vary.varmenu $vary.lowlabel  $vary.highlabel $vary.binsmenu $vary.unitlabel -sticky news
			grid $vary.varlabel $vary.low $vary.high $vary.bins $vary.unit -sticky news
		pack $vary -expand 1 -fill both -side right
	
	pack $bottom -expand 1 -fill both
	
	set slist $tops.slist
	set slistcolor lightgreen
	frame $slist -borderwidth 2 -relief sunken -background $slistcolor
	scrollbar $slist.vsb -orient vertical -command [list $slist.listbox yview] -background $slistcolor
	scrollbar $slist.hsb -orient horizontal -command [list $slist.listbox xview] -background $slistcolor
	
		set lbox $slist.listbox
		mclistbox::mclistbox $lbox -fillcolumn name -height 20 -width 100 \
		-labelanchor w -selectmode extended -labelborderwidth 1 -labelbg lightblue \
		-xscrollcommand [list $slist.hsb set] -yscrollcommand [list $slist.vsb set] -background $slistcolor \
		-columnrelief raised -columnborderwidth 1
		$lbox column add name -label "Name" -width 20
		$lbox column add type -label "Type" -width 5
		$lbox column add data -label "Data" -width 0
		$lbox column add xvar -label "X parameter" -width 15
		$lbox column add xlow -label "Low" -width 5
		$lbox column add xhig -label "High" -width 5
		$lbox column add xbin -label "Bins" -width 5
		$lbox column add yvar -label "Y parameter" -width 15
		$lbox column add ylow -label "Low" -width 5
		$lbox column add yhig -label "High" -width 5
		$lbox column add ybin -label "Bins" -width 5
		$lbox column add gate -label "Gate" -width 10

		$lbox label bind name <ButtonPress-1> "sort %W name"
		$lbox label bind type <ButtonPress-1> "sort %W type"
		$lbox label bind data <ButtonPress-1> "sort %W data"
		$lbox label bind xvar <ButtonPress-1> "sort %W xvar"
		$lbox label bind xbin <ButtonPress-1> "sort %W xbin"
		$lbox label bind yvar <ButtonPress-1> "sort %W yvar"
		$lbox label bind ybin <ButtonPress-1> "sort %W ybin"
		$lbox label bind gate <ButtonPress-1> "sort %W gate"
		bind $lbox <Double-ButtonPress-1> FillEntries
		
		grid $slist.vsb -in $slist -row 0 -column 1 -sticky ns
		grid $slist.hsb -in $slist -row 1 -column 0 -sticky ew
		grid $lbox -in $slist -row 0 -column 0 -sticky news -padx 0 -pady 0

	grid columnconfigure $slist 0 -weight 1
	grid columnconfigure $slist 1 -weight 0
	grid rowconfigure $slist 0 -weight 1
	grid rowconfigure $slist 1 -weight 0
	pack $slist -fill both -expand 1
	
	set display $tops.display
	set displaycolor lightblue
	frame $display -borderwidth 2 -background $displaycolor -relief groove
		button $display.update -text "Update Spectrum List" -command UpdateSpectrumList -width 40
		label $display.label -text "Spectrum mask"
		entry $display.mask -textvariable spectrumMask -width 30
		button $display.clear -text Clear -width 5 -command {set spectrumMask *}
		grid $display.update $display.label $display.mask $display.clear -sticky news
#		pack $display.update -side left -expand 1 -fill x
	pack $display -expand 1 -fill x
	pack $tops -expand 1 -fill both
}

proc SpectrumType1D {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state disabled
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text Parameter
	set wname $tops.bottom.vary
	$wname.varlabel configure -state disabled
	$wname.varmenu configure -state disabled
	$wname.low configure -state disabled
	$wname.high configure -state disabled
	$wname.bins configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state normal
}

proc SpectrumType2D {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state normal
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text "X Parameter"
	set wname $tops.bottom.vary
	$wname.varlabel configure -state normal
	$wname.varmenu configure -state normal -text "Y Parameter"
	$wname.low configure -state normal
	$wname.high configure -state normal
	$wname.bins configure -state normal
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeSummary {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state normal
	$wname.long configure -state normal
	set spectrumDatatype long
	set wname $tops.bottom.vary
	$tops.bottom.varx.varmenu configure -text Parameters
	$wname.varlabel configure -state disabled
	$wname.varmenu configure -state disabled
	$wname.low configure -state disabled
	$wname.high configure -state disabled
	$wname.bins configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeBitmask {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state disabled
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text Parameter
	set wname $tops.bottom.vary
	$wname.varlabel configure -state disabled
	$wname.varmenu configure -state disabled
	$wname.low configure -state disabled
	$wname.high configure -state disabled
	$wname.bins configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumType1G {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state disabled
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text Parameters
	set wname $tops.bottom.vary
	$wname.varlabel configure -state disabled
	$wname.varmenu configure -state disabled
	$wname.low configure -state disabled
	$wname.high configure -state disabled
	$wname.bins configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumType2G {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state normal
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text Parameters
	set wname $tops.bottom.vary
	$wname.varlabel configure -state disabled
	$wname.varmenu configure -state disabled
	$wname.low configure -state disabled
	$wname.high configure -state disabled
	$wname.bins configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeGP {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state normal
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text "X Parameter"
	set wname $tops.bottom.vary
	$wname.varlabel configure -state normal
	$wname.varmenu configure -state normal -text "Y Parameters"
	$wname.low configure -state normal
	$wname.high configure -state normal
	$wname.bins configure -state normal
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeS {} {
        global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype.n
	$wname.byte configure -state disabled
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text "Time Parameter"
	set wname $tops.bottom.vary
	$wname.varlabel configure -state normal
	$wname.varmenu configure -state normal -text "Count Parameter"
	$wname.low configure -state  disabled
	$wname.high configure -state disabled
	$wname.bins configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state normal

}

proc SpectrumParameterXCommand {p1 p2 p3} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global spectrumInfoX spectrumInfoY
	global spectrumLowX spectrumHighX spectrumBinsX spectrumUnitX
	global spectrumLowY spectrumHighY spectrumBinsY spectrumUnitY
	global tops
	set parameter $spectrumParameterX
	if {[GetParameterIndex $parameter] == -1} {
		set spectrumLowX ???
		set spectrumHighX ???
		set spectrumBinsX ???
		set spectrumUnitX ???
		return
	}
	set wname $tops.bottom.varx
	set spectrumLowX [GetParameterLow $parameter]
	set spectrumHighX [GetParameterHigh $parameter]
	set spectrumBinsX [GetParameterBins $parameter]
	set spectrumUnitX [GetParameterUnit $parameter]
	CreateBinsMenu $wname $spectrumBinsX spectrumBinsX
}

proc SpectrumParameterYCommand {p1 p2 p3} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global spectrumInfoX spectrumInfoY
	global spectrumLowX spectrumHighX spectrumBinsX spectrumUnitX
	global spectrumLowY spectrumHighY spectrumBinsY spectrumUnitY
	global tops
	set parameter $spectrumParameterY
	if {[GetParameterIndex $parameter] == -1} {
		set spectrumLowY ???
		set spectrumHighY ???
		set spectrumBinsY ???
		set spectrumUnitY ???
		return
	}
	set wname $tops.bottom.vary
	set spectrumLowY [GetParameterLow $parameter]
	set spectrumHighY [GetParameterHigh $parameter]
	set spectrumBinsY [GetParameterBins $parameter]
	set spectrumUnitY [GetParameterUnit $parameter]
	CreateBinsMenu $wname $spectrumBinsY spectrumBinsY
}

proc GetParameterIndex {parameter} {
	if {[string match $parameter ""]} {return}
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set arrayname [string range $parameter 0 [expr [string last . $parameter] - 1]]
	if {![info exist treeParameterName($arrayname)]} {return -1}
	set elementname [string range $parameter [expr [string last . $parameter] + 1] end]
	set index [lsearch $treeParameterName($arrayname) $elementname]
}

proc GetParameterLow {parameter} {
	if {[string match $parameter ""]} {return}
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set arrayname [string range $parameter 0 [expr [string last . $parameter] - 1]]
	set low [lindex $treeParameterStart($arrayname) [GetParameterIndex $parameter]]
}

proc GetParameterHigh {parameter} {
	if {[string match $parameter ""]} {return}
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set arrayname [string range $parameter 0 [expr [string last . $parameter] - 1]]
	set low [lindex $treeParameterStop($arrayname) [GetParameterIndex $parameter]]
}

proc GetParameterBins {parameter} {
	if {[string match $parameter ""]} {return}
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set arrayname [string range $parameter 0 [expr [string last . $parameter] - 1]]
	set low [lindex $treeParameterBins($arrayname) [GetParameterIndex $parameter]]
}

proc GetParameterUnit {parameter} {
	if {[string match $parameter ""]} {return}
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set arrayname [string range $parameter 0 [expr [string last . $parameter] - 1]]
	set low [lindex $treeParameterUnit($arrayname) [GetParameterIndex $parameter]]
}

proc GetParameterInfo {parameter} {
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	if {[string match $parameter ""]} {return}
	set arrayname [string range $parameter 0 [expr [string last . $parameter] - 1]]
	set elementname [string range $parameter [expr [string last . $parameter] + 1] end]
	set index [lsearch $treeParameterName($arrayname) $elementname]
	set info "[lindex $treeParameterStart($arrayname) $index] to [lindex $treeParameterStop($arrayname) $index] [lindex $treeParameterUnit($arrayname) $index]"
}

proc CreateResolutionMenu {wname resolution parameter} {
	destroy $wname.resmenu.menu
	menu $wname.resmenu.menu -tearoff 0
	for {set i $resolution} {$i >= 4} {incr i -1} {
		$wname.resmenu.menu add radiobutton -label "$i bits" -variable $parameter -value $i
	}
	$wname.resmenu configure -menu $wname.resmenu.menu
}

proc CreateBinsMenu {wname bins parameter} {
	destroy $wname.binsmenu.menu
	menu $wname.binsmenu.menu -tearoff 0
	set factors {100.0 50.0 20.0 10.0 5.0 2.0 1.0 0.5 0.2 0.1 0.05 0.02 0.01}
	foreach f $factors {
		set value [expr int($bins*$f)]
		$wname.binsmenu.menu add radiobutton -label $value -variable $parameter -value $value
	}
	$wname.binsmenu configure -menu $wname.binsmenu.menu
}

proc GenerateSpectrum {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global spectrumLowX spectrumHighX spectrumBinsX spectrumUnitX
	global spectrumLowY spectrumHighY spectrumBinsY spectrumUnitY
	
	if {[string match $spectrumName ""]} {
		tk_messageBox -icon error -message "Please provide a name for the spectrum" -title Error
		return
	}
	if {[string match $spectrumParameterX ""]} {
		tk_messageBox -icon error -message "Please select the X parameter" -title Error
		return
	}
	set spectrumList ""
	set theList [spectrum -list]
	foreach e $theList {
		append spectrumList [lindex $e 1] " "
	}
	switch  -- $spectrumType {
		1 {
			if {$spectrumArray} {
				set arrayname [string range $spectrumParameterX 0 [expr [string last . $spectrumParameterX] - 1]]
				foreach e $treeParameterName($arrayname) {
					if {[info exist treeParameterName($arrayname.$e)] == 0} {
						set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
						set spectrumResolutionList [list $spectrumAxisX]
						CreateSpectrum $spectrumList $spectrumName.$e $arrayname.$e $spectrumResolutionList
					}
				}
			} else {
				set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
				set spectrumResolutionList [list $spectrumAxisX]
				CreateSpectrum $spectrumList $spectrumName $spectrumParameterX $spectrumResolutionList
			}
		}
		2 {
			if {[string match $spectrumParameterY ""] && $spectrumType == 2} {
				tk_messageBox -icon error -message "Please select the Y parameter" -title Error
				return
			}
			set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
			set spectrumAxisY [list $spectrumLowY $spectrumHighY $spectrumBinsY]
			set spectrumParameterList [list $spectrumParameterX $spectrumParameterY]
			set spectrumResolutionList [list $spectrumAxisX $spectrumAxisY]
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
		s {
			set arrayname [string range $spectrumParameterX 0 [expr [string last . $spectrumParameterX] - 1]]
			foreach e $treeParameterName($arrayname) {
				if {[info exist treeParameterName($arrayname.$e)] == 0} {
					lappend spectrumParameterList $arrayname.$e
				}
			}
			set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
			set spectrumResolutionList [list $spectrumAxisX]
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
		b {
			set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
			set spectrumResolutionList [list $spectrumAxisX]
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterX $spectrumResolutionList
		}
		g1 {
			set arrayname [string range $spectrumParameterX 0 [expr [string last . $spectrumParameterX] - 1]]
			foreach e $treeParameterName($arrayname) {
				if {[info exist treeParameterName($arrayname.$e)] == 0} {
					lappend spectrumParameterList $arrayname.$e
				}
			}
			set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
			set spectrumResolutionList [list $spectrumAxisX]
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
		g2 {
			set arrayname [string range $spectrumParameterX 0 [expr [string last . $spectrumParameterX] - 1]]
			foreach e $treeParameterName($arrayname) {
				if {[info exist treeParameterName($arrayname.$e)] == 0} {
					lappend spectrumParameterList $arrayname.$e
				}
			}
			set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
			set spectrumResolutionList [list $spectrumAxisX $spectrumAxisX]
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
		gd {
			if {($spectrumParameterY eq "") && $spectrumType eq "gd"} {
				tk_messageBox -icon error -message "Please select the Y parameters" -title Error
				return
			}
			lappend spectrumParameterList $spectrumParameterX $spectrumParameterY
			set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
			set spectrumAxisY [list $spectrumLowY $spectrumHighY $spectrumBinsY]
			set spectrumResolutionList [list $spectrumAxisX $spectrumAxisY]
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
	        S {
		    set spectrumAxisX [list $spectrumLowX $spectrumHighX $spectrumBinsX]
		    set spectrumResolutionList [list $spectrumAxisX]
                    set spectrumParameterList [list $spectrumParameterX $spectrumParameterY]
		    CreateSpectrum $spectrumList $spectrumName $spectrumParameterList  $spectrumResolutionList
		} 
	}
	UpdateSpectrumList
	Modified
	sbind -all
}

proc CreateSpectrum {SpectrumList Name ParameterList ResolutionList} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	
	if {[lsearch $SpectrumList $Name] == -1} {
		spectrum $Name $spectrumType $ParameterList $ResolutionList $spectrumDatatype
		puts "Spectrum $Name created"
	} else {
#		if {[string match [tk_messageBox -icon warning -message "The spectrum $Name already exists.  Do you want to overwrite it?" \
		-title Warning -type yesno -parent .gui] yes]} {
			set gate [lindex [lindex [lindex [apply -list $Name] 0] 1] 0]
			spectrum -delete $Name
			spectrum $Name $spectrumType $ParameterList $ResolutionList $spectrumDatatype
			if {![string equal $gate -TRUE-]} {
				apply $gate $Name
			}
			puts "Spectrum $Name replaced"
#		}
	}
}

proc IsSpectrumArray {} {
	global spectrumName
	set suffix [lindex [split $spectrumName .] end]
	return [string is integer $suffix]
}

proc ListSpectrumArray {} {
	global spectrumName
	set theList [spectrum -list]
	set suffix [lindex [split $spectrumName .] end]
	set prefix [string trimright $spectrumName .$suffix]
	append prefix *
	set sList ""
	foreach s $theList {
		set name [lindex $s 1]
		if {[string match $prefix $name]} {
			lappend sList $name
		}
	}
	return $sList
}

proc GetSpectrumPrefix {} {
	global spectrumName
	set suffix [lindex [split $spectrumName .] end]
	set prefix [string trimright $spectrumName .$suffix]
	return $prefix
}

proc DynamicSpectrumList {name1 name2 op} {
	UpdateSpectrumList
}

proc UpdateSpectrumList {} {
    global tops spectrumMask
    set fraction [lindex [$tops.slist.listbox yview] 0]
    $tops.slist.listbox delete 0 end
    set theList [spectrum -list]
    set theApplyList [apply -list]
    foreach item $theApplyList {
	set spectrumname [lindex $item 0]
	set gatename     [lindex [lindex $item 1] 0]
	set Applications($spectrumname) $gatename
    }
    set i 0
    foreach spectrum $theList {
	set id [lindex $spectrum 0]
	set name [lindex $spectrum 1]
	if {[string match $spectrumMask $name] == 0} {
	    continue
	}
	set type [lindex $spectrum 2]
	set parameters [lindex $spectrum 3]
	set resolutions [lindex $spectrum 4]
	set data [lindex $spectrum 5]
	set gate $Applications($name)
	if {[string match $gate -TRUE-]} {set gate ""}
	if {[string match $gate -Ungated-]} {set gate ""}
	incr i
	switch -- $type {
	    1 {
		set ltype "1D"
		set varx $parameters
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary ""
		set lowy ""
		set highy ""
		set binsy ""
	    }
	    2 {
		set ltype "2D"
		set varx [lindex $parameters 0]
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary [lindex $parameters 1]
		set lowy [format %g [lindex [lindex $resolutions 1] 0]]
		set highy [format %g [lindex [lindex $resolutions 1] 1]]
		set binsy [lindex [lindex $resolutions 1] 2]
	    }
	    s {
		set ltype "Sum"
		set varx [lindex $parameters 0]
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary ""
		set lowy ""
		set highy ""
		set binsy ""
	    }
	    b {
		set ltype "Bit"
		set varx $parameters
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary ""
		set lowy ""
		set highy ""
		set binsy ""
	    }
	    g1 {
		set ltype "G1"
		set varx [lindex $parameters 0]
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary ""
		set lowy ""
		set highy ""
		set binsy ""
	    }
	    g2 {
		set ltype "G2"
		set varx [lindex $parameters 0]
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary ""
		set lowy ""
		set highy ""
		set binsy ""
	    }
	    gs {
		set ltype "GS"
		set varx [lindex [lindex $parameters 0] 0]
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binx  [format %x [lindex [lindex $resolutions 0] 2]]
		set vary ""
		set lowy ""
		set highy ""
		set binsy ""
			  
	    }
	    gd {
		set ltype "GD"
		set varx [lindex $parameters 0]
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary [lindex $parameters 1]
		set lowy [format %g [lindex [lindex $resolutions 1] 0]]
		set highy [format %g [lindex [lindex $resolutions 1] 1]]
		set binsy [lindex [lindex $resolutions 1] 2]
	    }
	    S {
		set ltype "Strip"
		set varx [lindex $parameters 0]
		set lowx [format %g [lindex [lindex $resolutions 0] 0]]
		set highx [format %g [lindex [lindex $resolutions 0] 1]]
		set binsx [lindex [lindex $resolutions 0] 2]
		set vary [lindex $parameters 1]
		set lowy ""
		set highy ""
		set binsy ""
	    }
	    default {
	    }
	}
	$tops.slist.listbox insert end [list $name $ltype $data $varx $lowx $highx $binsx $vary $lowy $highy $binsy $gate]
    }
    $tops.slist.listbox yview moveto $fraction
}

# sort the list based on a particular column
proc sort {w id} {
	set data [$w get 0 end]
	set index [lsearch -exact [$w column names] $id]

	if {[string match $id id] || [string match $id xbin] || [string match $id ybin]} {
		set result [lsort -integer -index $index $data]
	} else {
		set result [lsort -index $index $data]
	}

	$w delete 0 end

	# ... and add our sorted data in
	eval $w insert end $result
}

# Fill entries with the selected spectrum
proc FillEntries {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global treeParameterRoot treeParameterName treeParameterBins treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global spectrumInfoX spectrumInfoY spectrumGate
	global spectrumLowX spectrumHighX spectrumBinsX spectrumUnitX
	global spectrumLowY spectrumHighY spectrumBinsY spectrumUnitY
	global tops
	set lbox $tops.slist.listbox
	set index [lindex [$lbox curselection] 0]
	set spectrum [$lbox get $index]
	set spectrumName [lindex $spectrum 0]
	set type [lindex $spectrum 1]
	switch -- $type {
		1D {set type 1; SpectrumType1D}
		2D {set type 2; SpectrumType2D}
		Sum {set type s; SpectrumTypeSummary}
		Bit {set type b; SpectrumTypeBitmask}
		G1 {set type g1; SpectrumType1G}
		G2 {set type g2; SpectrumType2G}
		GD {set type gd; SpectrumTypeGP}
	}
	set spectrumArray 0
	if {[IsSpectrumArray]} {
		set spectrumName [GetSpectrumPrefix]
		set spectrumArray 1
	}
	set spectrumType $type
	set spectrumDatatype [lindex $spectrum 2]
	set spectrumParameterX [lindex $spectrum 3]
	set spectrumLowX [lindex $spectrum 4]
	set spectrumHighX [lindex $spectrum 5]
	set spectrumBinsX [lindex $spectrum 6]
    set spectrumUnitX [GetParameterUnit [lindex $spectrumParameterX 0]]  ;  # Could be several params.
	CreateBinsMenu $tops.bottom.varx $spectrumBinsX spectrumBinsX
	if {($type eq "2") || ($type eq "gd")} {
		set spectrumParameterY [lindex $spectrum 7]
		set spectrumLowY [lindex $spectrum 8]
		set spectrumHighY [lindex $spectrum 9]
		set spectrumBinsY [lindex $spectrum 10]
	    set spectrumUnitY [GetParameterUnit [lindex $spectrumParameterY 0]]
		CreateBinsMenu $tops.bottom.vary $spectrumBinsY spectrumBinsY
	}
	set spectrumGate [lindex $spectrum 11]
}

proc DeleteSpectra {} {
	global tops clearAll
	
	if {$clearAll} {
		if {[string compare no [tk_messageBox -message \
		"Are you sure you want to delete all spectra?" -type yesno]] == 0} {return}
		spectrum -delete -all
		return
	}
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	foreach index $selectedlist {
		set spectrum [$lbox get $index]
		spectrum -delete [lindex $spectrum 0]
	}
	UpdateSpectrumList
	Modified
}

proc ClearSpectra {} {
	global tops clearAll
	
	if {$clearAll} {
		clear -all
		return
	}
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	foreach index $selectedlist {
		set spectrum [$lbox get $index]
		clear [lindex $spectrum 0]
	}
}

proc DuplicateSpectra {} {
	global tops
	
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	set spectrumList [spectrum -list]
	foreach index $selectedlist {
		set copyNumber 0
		set spectrum [$lbox get $index]
		set spectrumName [lindex $spectrum 0]
		set exists 1
		while {$exists == 1} {
			incr copyNumber
			set exists 0
			set try $spectrumName
			append try _$copyNumber
			foreach s $spectrumList {
				if {[string compare [lindex $s 1] $try] == 0} {
					set exists 1
				}
			}
		}
	        set newSpectrum [lindex [spectrum -list $spectrumName] 0]
		set type [lindex $newSpectrum 2]
		set parameters [lindex $newSpectrum 3]
		set resolutions [lindex $newSpectrum 4]
		set datatype [lindex $newSpectrum 5]
		spectrum $try $type $parameters $resolutions $datatype
	}
	UpdateSpectrumList
	sbind -all
	Modified
}

proc GenerateGateMenu {parent command} {
# Generate a menu containing all available gates and attaches it to the parent widget
	destroy $parent.menu
	set wymax [winfo vrootheight .]
	menu $parent.menu -tearoff 0
	set theList [gate -list]
	foreach e $theList {
# If the menu has become too tall to fit on the root window we make a scrollable menu
		if {[$parent.menu yposition last] > $wymax-100} {
			$parent.menu insert 0 command -image uparrow -activebackground green
			$parent.menu add command -image downarrow -activebackground green
			bind $parent.menu <Enter> "ScrollGateMenu $parent.menu {$command}"
			bind $parent.menu <ButtonRelease-1> CancelScrollMenu
			bind $parent.menu <Leave> CancelScrollMenu
			break
		}
		set theName [lindex $e 0]
		set theType [lindex $e 2]
		if {[string compare $theType F] != 0} {
			$parent.menu add command -label $theName -command "$command {$theName}" \
			-activebackground yellow
		}
	}
	$parent configure -menu $parent.menu
}

proc ScrollGateMenu {wmenu command} {
	global afterScrollMenu
	set afterScrollMenu [after 20 "ScrollGateMenu $wmenu {$command}"]
	set activeItem [$wmenu index active]
	if {$activeItem != 0 && $activeItem != [$wmenu index last]} {
		update
		return
	}

# find indexes of first and last
	set i 0
	set lastItem [expr [$wmenu index last]-1]
	set gateList [gate -list]
	foreach e $gateList {
		lappend theList [lindex $e 0]
	}
	foreach entry $theList {
		if {[string equal [$wmenu entrycget 1 -label] $entry]} {set indexFirst $i}
		if {[string equal [$wmenu entrycget $lastItem -label] $entry]} {set indexLast $i}
		incr i
	}

# Process Down arrow
	if {$activeItem == [$wmenu index last]} {
# if the last menu item is on the last entry we do nothing and return
		if {$indexLast+1 == [llength $theList]} {return}
# For the main body of the menu, shift all items up
		for {set i 1} {$i < $lastItem} {incr i} {
			$wmenu entryconfigure $i -label [$wmenu entrycget [expr $i+1] -label]
			$wmenu entryconfigure $i -command [$wmenu entrycget [expr $i+1] -command]
		}
# The last item gets the new entry
		set newEntry [lindex $theList [expr $indexLast+1]]
		$wmenu entryconfigure $lastItem -label $newEntry -command "$command $newEntry" \
		-activebackground yellow
	}

# Process Up arrow
	if {$activeItem == 0} {
# if the first menu item is on the first entry we do nothing and return
		if {$indexFirst == 0} {return}
# For the main body of the menu, shift all items down
		for {set i $lastItem} {$i > 1} {incr i -1} {
			$wmenu entryconfigure $i -label [$wmenu entrycget [expr $i-1] -label]
			$wmenu entryconfigure $i -command [$wmenu entrycget [expr $i-1] -command]
		}
# The first item gets the new entry
		set newEntry [lindex $theList [expr $indexFirst-1]]
		$wmenu entryconfigure 1 -label $newEntry -command "$command $newEntry" \
		-activebackground yellow
	}
}

proc SpectrumGateCommand {theGate} {
	global spectrumGate
	set spectrumGate $theGate
	ApplyGate
}

proc ApplyGate {} {
	global spectrumGate
	global tops
	
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	if {[string match $spectrumGate ""]} {return}
	foreach index $selectedlist {
		set spectrum [$lbox get $index]
		set name [lindex $spectrum 0]
		apply $spectrumGate $name
	}
	UpdateSpectrumList
	Modified
}

proc UngateSpectra {} {
	global tops
	
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	foreach index $selectedlist {
		set spectrum [$lbox get $index]
		set name [lindex $spectrum 0]
# The catch command is used because the ungate command was returning an error for no reason
		catch {ungate $name}
	}
	UpdateSpectrumList
	Modified
}

proc Modified {} {
	global definitionFile enableFailsafe
	if {[string compare [lindex $definitionFile 1] (modified)] != 0} {
		set definitionFile "$definitionFile (modified)"
	}
	if {$enableFailsafe} {
		SaveDefinitionFile failsafe.tcl
	}
}

proc Save {} {
	global definitionFile fullDefinitionFile definitionDirectory
	
	if {[string compare $definitionDirectory ""] == 0} {set definitionDirectory [pwd]}
	set fullDefinitionFile [tk_getSaveFile -filetypes {\
	{"Tcl script file" {.tcl}}\
	{"Definition file" {.sdef}}\
	}\
	-initialdir $definitionDirectory -initialfile [lindex $definitionFile 0] \
	-defaultextension .tcl \
	-title "Save Spectrum Definition file" -parent .gui]
	if {[string match $fullDefinitionFile ""]} {return}
	set definitionFile [lindex [split $fullDefinitionFile /] end]
	set definitionDirectory [string trimright $fullDefinitionFile $definitionFile]

	SaveDefinitionFile $fullDefinitionFile
}

proc Load {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global spectrumInfoX spectrumInfoY
	global definitionFile fullDefinitionFile definitionDirectory deleteAll
	
	set fullDefinitionFile [tk_getOpenFile -filetypes {\
	{"Tcl Script file" {.tcl}}\
	{"Definition file" {.sdef}}\
	}\
	-initialdir $definitionDirectory -title "Load Spectrum Definition file" -parent .gui]
	if {[string match $fullDefinitionFile ""]} {return}
	set definitionFile [lindex [split $fullDefinitionFile /] end]
	set definitionDirectory [string trimright $fullDefinitionFile $definitionFile]
	if {$deleteAll == 0} {
		spectrum -delete -all
	}
	LoadDefinitionFile $fullDefinitionFile
	UpdateTreeParameters
	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
	set spectrumInfoY [GetParameterInfo $spectrumParameterY]
	UpdateGateList
	UpdateSpectrumList
}

proc LoadDefinitionFile {file} {
	global parameter variable
	set handle [open $file r]
	set definitionFile [lindex [split $file /] end]
	set extension [lindex [split $definitionFile .] end]

	set currentSpectra [spectrum -list]
	foreach e $currentSpectra {
		append spectrumList [lindex $e 1] " "
	}

# Old style definition file is just a list of lists
	if {[string compare $extension sdef] == 0} {
		set theList [read $handle]
		close $handle
		set theSpectrumList [lindex $theList 0]
		set theTreeParameterList [lindex $theList 1]
		set theGateList [lindex $theList 2]
		set theApplyList [lindex $theList 3]
		foreach spectrum $theSpectrumList {
			set id [lindex $spectrum 0]
			set name [lindex $spectrum 1]
			set type [lindex $spectrum 2]
			set parameters [lindex $spectrum 3]
			set resolutions [lindex $spectrum 4]
			set data [lindex $spectrum 5]
# We need to check if spectra definitions attempt to redefine existing spectra
# and delete them if it is the case (no implicit overwriting)
			if {[info exist spectrumList] == 1 && [lsearch $spectrumList $name] != -1} {
				spectrum -delete $name
			}
			spectrum $name $type $parameters $resolutions $data
		}
		foreach treeparameter $theTreeParameterList {
			set name [lindex $treeparameter 0]
			set start [lindex $treeparameter 2]
			set stop [lindex $treeparameter 3]
			set increment [lindex $treeparameter 4]
			set unit [lindex $treeparameter 5]
		    #  WRONG WRONG WRONG treeparameter -set $name $start $stop $increment $unit
		    treeparameter -setunit   $name $unit
		    treeparameter -setlimits $name $start $stop
		    treeparameter -setinc    $name $increment
		}
		foreach gate $theGateList {
			set name [lindex $gate 0]
			set type [lindex $gate 2]
			set description [lindex $gate 3]
			set parameters [lindex $description 0]
			set data [lreplace $description 0 0]
			set gatecmd "gate $name"
			switch -- $type {
				c -
				b {
					set des "[lindex $parameters 0] [lindex $parameters 1] \{$data\}"
					lappend gatecmd $type $des
				}
				s {
					set des "[lindex $parameters 0] $data"
					lappend gatecmd $type $des
				}
				default {
					lappend gatecmd $type $description
				}
			}
			eval $gatecmd
		}
		foreach apply $theApplyList {
			set spectrum [lindex $apply 0]
			set gate [lindex $apply 1]
			set gatename [lindex $gate 0]
			set gatetype [lindex $gate 2]
			if {[string compare $gatetype T] != 0} {
				apply $gatename $spectrum
			}
		}
	}

# New style definition file is a Tcl script
	if {[string compare $extension tcl] == 0} {
		while {![eof $handle]} {
			gets $handle line
# We need to check if spectra definitions attempt to redefine existing spectra
# and delete them if it is the case (no implicit overwriting)
			if {[string compare spectrum [lindex $line 0]] == 0} {
				set name [lindex $line 1]
				if {[info exist spectrumList] == 1 && [lsearch $spectrumList $name] != -1} {
					spectrum -delete $name
				}
			}
# That's the nice thing about saving a script!
			eval $line
		}
		close $handle
	}
	UpdateSpectrumList
	UpdateGateList
	sbind -all
}

proc SaveDefinitionFile {file} {
	global parameter variable
	set handle [open $file w]
	set definitionFile [lindex [split $file /] end]

	set spectrumList [spectrum -list]

	set treeList [treeparameter -list]
	set changedTreeList ""
	foreach tree $treeList {
		if {[treeparameter -check [lindex $tree 0]]} {
			append changedTreeList [treeparameter -list [lindex $tree 0]] " "
		}
	}
	set treeparameterList $changedTreeList
	set treeList [treevariable -list]
	set changedTreeList ""
	foreach tree $treeList {
		if {[treevariable -check [lindex $tree 0]]} {
			append changedTreeList [treevariable -list [lindex $tree 0]] " "
		}
	}
	set treevariableList $changedTreeList
	
        set gateList [SortGates]
	set applyList [apply -list]

	set extension [lindex [split $definitionFile .] end]
	if {[string compare $extension sdef] == 0} {
		set theList ""
		lappend theList $spectrumList
		lappend theList $treeparameterList
		lappend theList $gateList
		lappend theList $applyList
		puts $handle $theList
	}
	if {[string compare $extension tcl] == 0} {
		puts $handle "# Definition file saved [clock format [clock seconds]]"
		puts $handle "# Spectrum definitions"
		foreach spectrum $spectrumList {
			set id [lindex $spectrum 0]
			set name [lindex $spectrum 1]
			set type [lindex $spectrum 2]
			set parameters [lindex $spectrum 3]
			set resolutions [lindex $spectrum 4]
			set data [lindex $spectrum 5]
			puts $handle "spectrum $name $type \{$parameters\} \{$resolutions\} $data"
		}
		puts $handle "# Gate definitions"
		foreach gate $gateList {
			set name [lindex $gate 0]
			set type [lindex $gate 2]
			set description [lindex $gate 3]
			set parameters [lindex $description 0]
			set data [lreplace $description 0 0]
			set gatecmd "gate \{$name\}"
			switch -- $type {
				c -
				b {
					set des "[lindex $parameters 0] [lindex $parameters 1] \{$data\}"
					lappend gatecmd $type $des
				}
				s {
					set des "[lindex $parameters 0] $data"
					lappend gatecmd $type $des
				}
				default {
					lappend gatecmd $type $description
				}
			}
			puts $handle $gatecmd
		}
		puts $handle "# Apply definitions"
		foreach apply $applyList {
			set spectrum [lindex $apply 0]
			set gate [lindex $apply 1]
			set gatename [lindex $gate 0]
			set gatetype [lindex $gate 2]
			if {![string equal $gatetype T] && ![string equal $gatetype F]} {
				puts $handle "apply $gatename $spectrum"
			}
		}
	        puts $handle "# Gates applied as folds"
	        foreach fold [fold -list] {
		    puts $handle "fold -apply [list [lindex $fold 1]] [list [lindex $fold 0]]"
	        }
		puts $handle "# TreeParameter changes from C++ code"
		foreach treeparameter $treeparameterList {
			set name [lindex $treeparameter 0]
			set bins [lindex $treeparameter 1]
			set start [lindex $treeparameter 2]
			set stop [lindex $treeparameter 3]
			set increment [lindex $treeparameter 4]
			set unit [lindex $treeparameter 5]
		    puts $handle "[list treeparameter -set $name $bins $start $stop $increment $unit]"
		}
		puts $handle "# TreeVariable changes from C++ code"
		foreach treevariable $treevariableList {
			set name [lindex $treevariable 0]
			set value [lindex $treevariable 1]
			set unit [lindex $treevariable 2]
		    puts $handle "[list treevariable -set $name $value $unit]"
		}
		puts $handle "# Parameter page"
		foreach p [array names parameter] {
			if {[string equal $parameter($p) ""] == 0} {
				puts $handle "set parameter($p) $parameter($p)"
			}
		}
		puts $handle "# Variable page"
		foreach v [array names variable] {
			if {[string equal $variable($v) ""] == 0} {
				puts $handle "set variable($v) $variable($v)"
			}
		}
	}
	close $handle
}

proc SortGates {} {
	catch {unset ::SortedGates}
	catch {unset ::SearchList}
	set ::SortedGates [list]
	set GateList [gate -list]
	foreach gate $GateList {
		lappend ::SearchList $gate
	}
	set newlist [Sort $GateList]
	return $newlist
}

proc Sort {List} {
	foreach gate $List {
		if {[lsearch $::SortedGates $gate] == -1} {
			write $gate;
		}
	}
        return $::SortedGates
}

proc write gate {
        if {[DependsOnDeleted $gate]} {
            return 0
	}
	set DependsOn [GetDependencies $gate] 
	if { $DependsOn != ""} {
	    foreach Dependent $DependsOn {
		   foreach look_for_gate $::SearchList {
			 if {[lindex $look_for_gate 0] == $Dependent} {
			       if {[lsearch $::SortedGates $look_for_gate]==-1} {
			             write $look_for_gate
                               }
                         }
		    }
	    }
            lappend ::SortedGates $gate
	} else {
	    lappend ::SortedGates $gate
	}
}

proc GetDependencies {def} {
	set temp [lindex $def 2]
	if {$temp=="s"||$temp=="c"||$temp=="b"||$temp=="gs"||$temp=="gb"||$temp=="gc"||$temp=="T"||$temp=="F"} {
		return ""
        } else {
		return [lindex $def 3]
	}
}

proc DependsOnDeleted gate {   
    set Dependents [GetDependencies $gate]
    if { $Dependents == ""} {
	if { [lindex $gate 2] == "F"} {	
	    return 1
	} else {
	    return false
	}
    } else {
	if {([lindex $gate 2] == "em") || \
	    ([lindex $gate 2] == "am") || \
	    ([lindex $gate 2] == "nm") } {
	    return false
	}
	foreach Dependent $Dependents {
	    foreach lookup_gate $::SearchList {
		if {[lindex $lookup_gate 0] == $Dependent} {
		    if {[DependsOnDeleted $lookup_gate]} {
			return 1
		    } else {
			return 0
		    }
		}  
	    }
	}
    }  
}

proc lswap {list index1 index2} {
	set el1 [lindex $list $index1]
	set el2 [lindex $list $index2]
	set newlist [lreplace $list $index1 $index1 $el2]
	set newlist [lreplace $newlist $index2 $index2 $el1]
	return $newlist
}
