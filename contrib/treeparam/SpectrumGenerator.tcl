# SpectrumGenerator.tcl
# Tcl/Tk script to generate a menu driven spectrum creation
# Author: D. Bazin
# Date: Dec 2000

source /usr/TruCluster/users/bazin/daq/parameter/TreeParameter.tcl
source /usr/TruCluster/users/bazin/tcltk/mclistbox/mclistbox.tcl

proc SetupSpectrumGenerator {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global definitionFile tops definitionDirectory
	UpdateTreeParameters
	CreateSpectrumGenerator
	set spectrumType 1
	SpectrumType1D
	set spectrumDatatype long
	set wname $tops.bottom.varx.varmenu
	GenerateTreeMenu $wname SpectrumParameterXCommand
	set wname $tops.bottom.vary.varmenu
	GenerateTreeMenu $wname SpectrumParameterYCommand
	set wname $tops.middle.gate.menu
	GenerateGateMenu $wname SpectrumGateCommand
	set definitionFile Unknown
	set definitionDirectory [pwd]
	UpdateSpectrumList
}

proc CreateSpectrumGenerator {} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global spectrumInfoX spectrumInfoY
	global definitionFile tops
	if {[winfo exists .theTop]} {
		set tops .theTop.spectrumgenerator
		frame $tops -borderwidth 8 -relief groove
	} else {
		set tops .spectrumgenerator
		toplevel $tops
		wm title $tops "Spectrum Generator"
	}
	
	set options $tops.options
	set optionscolor lightblue
	frame $options

		set type $options.type
		frame $type -borderwidth 4 -relief groove -background $optionscolor
			label $type.label -text "Spectrum type" -background $optionscolor
			radiobutton $type.1d -text "1D" -variable spectrumType -value 1 -command SpectrumType1D -background $optionscolor
			radiobutton $type.2d -text "2D" -variable spectrumType -value 2 -command SpectrumType2D -background $optionscolor
			radiobutton $type.sum -text "Summary" -variable spectrumType -value s -command SpectrumTypeSummary -background $optionscolor
			radiobutton $type.bit -text "Bitmask" -variable spectrumType -value b -command SpectrumTypeBitmask -background $optionscolor
			pack $type.label
			pack $type.1d $type.2d $type.sum $type.bit -anchor w
		pack $type -expand 1 -fill both -side left
	
		set datatype $options.datatype
		frame $datatype -borderwidth 4 -relief groove -background $optionscolor
			label $datatype.label -text "Data type" -background $optionscolor
			radiobutton $datatype.byte -text "Byte (8 bits)" -variable spectrumDatatype -value byte -background $optionscolor
			radiobutton $datatype.word -text "Word (16 bits)" -variable spectrumDatatype -value word -background $optionscolor
			radiobutton $datatype.long -text "Long (32 bits)" -variable spectrumDatatype -value long -background $optionscolor
			pack $datatype.label
			pack $datatype.byte $datatype.word $datatype.long -anchor w
		pack $datatype -expand 1 -fill both -side left
		
		set control $options.control
		frame $control -borderwidth 4 -relief groove -background $optionscolor
			label $control.label -text "Definition file"  -background $optionscolor
			button $control.load -text "Load" -command LoadSpectrumDefinition  -background $optionscolor
			button $control.save -text "Save" -command SaveSpectrumDefinition  -background $optionscolor
			label $control.file -textvariable definitionFile  -background $optionscolor
			pack $control.label $control.file $control.load $control.save
		pack $control -expand 1 -fill both -side right

	pack $options -expand 1 -fill both
	
	set middle $tops.middle
	set middlecolor lightyellow
	frame $middle -background $middlecolor

		set command $middle.command
		frame $command -borderwidth 4 -relief groove -background $middlecolor
			label $command.label -text "Spectrum name" -background $middlecolor
			entry $command.entry -width 20 -textvariable spectrumName -background $middlecolor
			button $command.generate -text "Generate" -command GenerateSpectrum -background $middlecolor
			checkbutton $command.generatearray -text "Array" -variable spectrumArray -background $middlecolor
			button $command.delete -text "Delete" -command DeleteSpectra -background $middlecolor
			button $command.duplicate -text "Duplicate" -command DuplicateSpectra -background $middlecolor
			grid $command.label $command.generate $command.delete -sticky news
			grid $command.entry $command.generatearray $command.duplicate -sticky news
		pack $command -expand 1 -fill both -side left

		set gate $middle.gate
		frame $gate -borderwidth 4 -relief groove -background $middlecolor
			label $gate.label -width 10 -textvariable spectrumGate -background $middlecolor
			menubutton $gate.menu -text "Gate" -background $middlecolor
			button $gate.apply -text "Apply" -command ApplyGate -background $middlecolor
			button $gate.ungate -text "Ungate" -command UngateSpectra -background $middlecolor
			grid $gate.label $gate.apply -sticky news
			grid $gate.menu $gate.ungate -sticky news
		pack $gate -expand 1 -fill both -side right
		bind $gate.menu <ButtonPress-1> "GenerateGateMenu $gate.menu SpectrumGateCommand"

	pack $middle -expand 1 -fill both
	
	set bottom $tops.bottom
	set bottomcolor pink
	frame $bottom -background $bottomcolor
	
		set varx $bottom.varx
		frame $varx -borderwidth 4 -relief groove -background $bottomcolor
			label $varx.varlabel -textvariable spectrumParameterX -background $bottomcolor
			menubutton $varx.varmenu -text "Parameter X" -background $bottomcolor
			label $varx.infolabel -textvariable spectrumInfoX -background $bottomcolor
			label $varx.reslabel -textvariable spectrumResolutionX -background $bottomcolor
			menubutton $varx.resmenu -text "Resolution" -background $bottomcolor
			grid $varx.varlabel $varx.infolabel x -sticky news
			grid $varx.varmenu $varx.resmenu $varx.reslabel -sticky news
		pack $varx -expand 1 -fill both -side left
		
		set vary $bottom.vary
		frame $vary -borderwidth 4 -relief groove -background $bottomcolor
			label $vary.varlabel -textvariable spectrumParameterY -background $bottomcolor
			menubutton $vary.varmenu -text "Parameter Y" -background $bottomcolor
			label $vary.infolabel -textvariable spectrumInfoY -background $bottomcolor
			label $vary.reslabel -textvariable spectrumResolutionY -background $bottomcolor
			menubutton $vary.resmenu -text "Resolution" -background $bottomcolor
			grid $vary.varlabel $vary.infolabel  x -sticky news
			grid $vary.varmenu $vary.resmenu $vary.reslabel -sticky news
		pack $vary -expand 1 -fill both -side right
	
	pack $bottom -expand 1 -fill both
	
	set slist $tops.slist
	set slistcolor lightgreen
	frame $slist -borderwidth 4 -relief sunken -background $slistcolor
	scrollbar $slist.vsb -orient vertical -command [list $slist.listbox yview] -background $slistcolor
	scrollbar $slist.hsb -orient horizontal -command [list $slist.listbox xview] -background $slistcolor
	
		set lbox $slist.listbox
		mclistbox::mclistbox $lbox -fillcolumn name -height 10 -width 60 \
		-labelanchor w -selectmode extended -labelborderwidth 2 \
		-xscrollcommand [list $slist.hsb set] -yscrollcommand [list $slist.vsb set] -background $slistcolor
		$lbox column add name -label "Name" -width 15
#		$lbox column add id -label "Id" -width 4
		$lbox column add type -label "Type" -width 0
		$lbox column add data -label "Data" -width 0
		$lbox column add xvar -label "X parameter" -width 15
		$lbox column add xbit -label "bits" -width 4
		$lbox column add yvar -label "Y parameter" -width 15
		$lbox column add ybit -label "bits" -width 4
		$lbox column add gate -label "Gate" -width 10

		$lbox label bind name <ButtonPress-1> "sort %W name"
#		$lbox label bind id <ButtonPress-1> "sort %W id"
		$lbox label bind type <ButtonPress-1> "sort %W type"
		$lbox label bind data <ButtonPress-1> "sort %W data"
		$lbox label bind xvar <ButtonPress-1> "sort %W xvar"
		$lbox label bind xbit <ButtonPress-1> "sort %W xbit"
		$lbox label bind yvar <ButtonPress-1> "sort %W yvar"
		$lbox label bind ybit <ButtonPress-1> "sort %W ybit"
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

	if {[winfo exists .theTop]} {
		pack $tops -expand 1 -fill both
	}
}

proc SpectrumType1D {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state disabled
	$wname.long configure -state normal
#	if {[string match $spectrumDatatype byte]} {
		set spectrumDatatype long
#	}
	set wname $tops.bottom.vary
	$wname.varmenu configure -state disabled
#	$wname.varlabel configure -state disabled
	$wname.resmenu configure -state disabled
#	$wname.reslabel configure -state disabled
#	$wname.infolabel configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state normal
}

proc SpectrumType2D {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state normal
	$wname.long configure -state disabled
#	if {[string match $spectrumDatatype long]} {
		set spectrumDatatype word
#	}
	set wname $tops.bottom.vary
	$wname.varmenu configure -state normal
#	$wname.varlabel configure -state normal
	$wname.resmenu configure -state normal
#	$wname.reslabel configure -state normal
#	$wname.infolabel configure -state normal
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeSummary {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state normal
	$wname.long configure -state disabled
#	if {[string match $spectrumDatatype long]} {
		set spectrumDatatype word
#	}
	set wname $tops.bottom.vary
	$wname.varmenu configure -state disabled
	$wname.resmenu configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeBitmask {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state disabled
	$wname.long configure -state normal
#	if {[string match $spectrumDatatype byte]} {
		set spectrumDatatype long
#	}
	set wname $tops.bottom.vary
	$wname.varmenu configure -state disabled
	$wname.resmenu configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumParameterXCommand {parameter} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global spectrumInfoX spectrumInfoY
	global tops
	set spectrumParameterX $parameter
	set wname $tops.bottom.varx
	set spectrumResolutionX [GetParameterResolution $parameter]
	set spectrumInfoX [GetParameterInfo $parameter]
	CreateResolutionMenu $wname $spectrumResolutionX spectrumResolutionX
}

proc SpectrumParameterYCommand {parameter} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global spectrumInfoX spectrumInfoY
	global tops
	set spectrumParameterY $parameter
	set wname $tops.bottom.vary
	set spectrumResolutionY [GetParameterResolution $parameter]
	set spectrumInfoY [GetParameterInfo $parameter]
	CreateResolutionMenu $wname $spectrumResolutionY spectrumResolutionY
}

proc GetParameterResolution {parameter} {
	if {[string match $parameter ""]} {return}
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	set arrayname [string range $parameter 0 [expr [string last . $parameter] - 1]]
	set elementname [string range $parameter [expr [string last . $parameter] + 1] end]
	set index [lsearch $treeParameterName($arrayname) $elementname]
	set resolution [lindex $treeParameterBits($arrayname) $index]
}

proc GetParameterInfo {parameter} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
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

proc GenerateSpectrum {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
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
						CreateSpectrum $spectrumList $spectrumName.$e $arrayname.$e $spectrumResolutionX
					}
				}
			} else {
				CreateSpectrum $spectrumList $spectrumName $spectrumParameterX $spectrumResolutionX
			}
		}
		2 {
			if {[string match $spectrumParameterY ""] && $spectrumType == 2} {
				tk_messageBox -icon error -message "Please select the Y parameter" -title Error
				return
			}
			append spectrumParameterList $spectrumParameterX " " $spectrumParameterY
			append spectrumResolutionList $spectrumResolutionX " " $spectrumResolutionY
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
		s {
			set arrayname [string range $spectrumParameterX 0 [expr [string last . $spectrumParameterX] - 1]]
			foreach e $treeParameterName($arrayname) {
				if {[info exist treeParameterName($arrayname.$e)] == 0} {
					append spectrumParameterList "$arrayname.$e "
				}
			}
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionX
		}
		b {
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterX $spectrumResolutionX
		}
	}
	UpdateSpectrumList
	sbind -all
}

proc CreateSpectrum {SpectrumList Name ParameterList ResolutionList} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	if {[lsearch $SpectrumList $Name] == -1} {
		spectrum $Name $spectrumType $ParameterList $ResolutionList $spectrumDatatype
		puts "Spectrum $Name created"
	} else {
		if {[string match [tk_messageBox -icon warning -message "The spectrum $Name already exists.  Do you want to overwrite it?" \
		-title Warning -type yesno] yes]} {
			spectrum -delete $Name
			spectrum $Name $spectrumType $ParameterList $ResolutionList $spectrumDatatype
			puts "Spectrum $Name replaced"
		}
	}
}

proc UpdateSpectrumList {} {
	global tops
	$tops.slist.listbox delete 0 end
	set theList [spectrum -list]
	foreach spectrum $theList {
		set id [lindex $spectrum 0]
		set name [lindex $spectrum 1]
		set type [lindex $spectrum 2]
		set parameters [lindex $spectrum 3]
		set resolutions [lindex $spectrum 4]
		set data [lindex $spectrum 5]
		set gate [lindex [lindex [lindex [apply -list $name] 0] 1] 0]
		if {[string match $gate -TRUE-]} {set gate ""}
		if {[string match $gate -Ungated-]} {set gate ""}
		switch -- $type {
			1 {
				set ltype "1D"
				set varx $parameters
				set resx $resolutions
				set vary ""
				set resy ""
			}
			2 {
				set ltype "2D"
				set varx [lindex $parameters 0]
				set resx [lindex $resolutions 0]
				set vary [lindex $parameters 1]
				set resy [lindex $resolutions 1]
			}
			s {
				set ltype "Sum"
				set varx [lindex $parameters 0]
				set resx $resolutions
				set vary ""
				set resy ""
			}
			b {
				set ltype "Bit"
				set varx $parameters
				set resx $resolutions
				set vary ""
				set resy ""
			}
		}
#		$tops.slist.listbox insert end [list $name $id $ltype $data $varx $resx $vary $resy]
		$tops.slist.listbox insert end [list $name $ltype $data $varx $resx $vary $resy $gate]
	}
}

# sort the list based on a particular column
proc sort {w id} {
	set data [$w get 0 end]
	set index [lsearch -exact [$w column names] $id]

	if {[string match $id id] || [string match $id xbit] || [string match $id ybit]} {
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
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global spectrumInfoX spectrumInfoY spectrumGate
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
	}
	set spectrumType $type
	set spectrumDatatype [lindex $spectrum 2]
	set spectrumParameterX [lindex $spectrum 3]
	set spectrumResolutionX [lindex $spectrum 4]
	CreateResolutionMenu $tops.bottom.varx [GetParameterResolution $spectrumParameterX] spectrumResolutionX
	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
	if {$type == 2} {
		set spectrumParameterY [lindex $spectrum 5]
		set spectrumResolutionY [lindex $spectrum 6]
		CreateResolutionMenu $tops.bottom.vary [GetParameterResolution $spectrumParameterY] spectrumResolutionY
		set spectrumInfoY [GetParameterInfo $spectrumParameterY]
	}
	set spectrumGate [lindex $spectrum 7]
}

proc DeleteSpectra {} {
	global tops
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	foreach index $selectedlist {
		set spectrum [$lbox get $index]
		spectrum -delete [lindex $spectrum 0]
	}
	UpdateSpectrumList
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
		set newSpectrum [spectrum -list $spectrumName]
		set type [lindex $newSpectrum 2]
		set parameters [lindex $newSpectrum 3]
		set resolutions [lindex $newSpectrum 4]
		set datatype [lindex $newSpectrum 5]
		spectrum $try $type $parameters $resolutions $datatype
	}
	UpdateSpectrumList
}
	
proc SaveSpectrumDefinition {} {
	global definitionFile fullDefinitionFile definitionDirectory
	if {[string compare $definitionDirectory ""] == 0} {set definitionDirectory [pwd]}
	set fullDefinitionFile [tk_getSaveFile -filetypes {{"Spectrum definition file" {.sdef .def}}} \
	-initialdir $definitionDirectory -initialfile $definitionFile -defaultextension .sdef \
	-title "Save Spectrum Definition file"]
	if {[string match $fullDefinitionFile ""]} {return}
	set definitionFile [lindex [split $fullDefinitionFile /] end]
	set definitionDirectory [string trimright $fullDefinitionFile "/$definitionFile"]
	set handle [open $fullDefinitionFile w]
	set theList ""
	lappend theList [spectrum -list]
	lappend theList [treeparameter -list]
	lappend theList [gate -list]
	lappend theList [apply -list]
	puts $handle $theList
	close $handle
}

proc LoadSpectrumDefinition {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global spectrumInfoX spectrumInfoY
	global definitionFile fullDefinitionFile definitionDirectory
	set fullDefinitionFile [tk_getOpenFile -filetypes {{"Spectrum definition file" {.sdef .def}}} \
	-initialdir $definitionDirectory -title "Load Spectrum Definition file"]
	if {[string match $fullDefinitionFile ""]} {return}
	set definitionFile [lindex [split $fullDefinitionFile /] end]
	set handle [open $fullDefinitionFile r]
	set theList [read $handle]
	close $handle
	set theSpectrumList [lindex $theList 0]
	set theTreeParameterList [lindex $theList 1]
	set theGateList [lindex $theList 2]
	set theApplyList [lindex $theList 3]
	set currentSpectra [spectrum -list]
	foreach e $currentSpectra {
		append spectrumList [lindex $e 1] " "
	}
#	spectrum -delete -all
	foreach spectrum $theSpectrumList {
		set id [lindex $spectrum 0]
		set name [lindex $spectrum 1]
		set type [lindex $spectrum 2]
		set parameters [lindex $spectrum 3]
		set resolutions [lindex $spectrum 4]
		set data [lindex $spectrum 5]
		if {[info exist spectrumList] == 1 && [lsearch $spectrumList $name] != -1} {spectrum -delete $name}
		spectrum $name $type $parameters $resolutions $data
	}
	sbind -all
	foreach treeparameter $theTreeParameterList {
		set name [lindex $treeparameter 0]
		set start [lindex $treeparameter 2]
		set stop [lindex $treeparameter 3]
		set increment [lindex $treeparameter 4]
		set unit [lindex $treeparameter 5]
		treeparameter -set $name $start $stop $increment $unit
	}
	UpdateTreeParameters
	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
	set spectrumInfoY [GetParameterInfo $spectrumParameterY]
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
	UpdateSpectrumList
}

proc GenerateGateMenu {parent command} {
# Generate a menu containing all available gates and attaches it to the parent widget
	destroy $parent.menu
	menu "$parent.menu" -tearoff 0
	set theList [gate -list]
	foreach e $theList {
		set theName [lindex $e 0]
		set theType [lindex $e 2]
		if {[string compare $theType F] != 0} {
			"$parent.menu" add command -label $theName -command "$command {$theName}"
		}
	}
	$parent configure -menu "$parent.menu"
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
}
