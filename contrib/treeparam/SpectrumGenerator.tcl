# SpectrumGenerator.tcl
# Tcl/Tk script to generate a menu driven spectrum creation
# Author: D. Bazin
# Date: Dec 2000 - Sept 2002

proc SetupSpectrumGenerator {parent} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global definitionFile tops definitionDirectory
	UpdateTreeParameters
	CreateSpectrumGenerator $parent
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

proc CreateSpectrumGenerator {parent} {
	global treeParameterRoot treeParameterName treeParameterBits treeParameterStart treeParameterStop treeParameterInc treeParameterUnit
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	global spectrumInfoX spectrumInfoY ActivityMonitor
	global definitionFile tops deleteAll clearAll
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
			frame $type.g -background $optionscolor
			radiobutton $type.g.1g -text "Gamma1D" -variable spectrumType -value g1 -command SpectrumType1G -background $optionscolor
			radiobutton $type.g.2g -text "Gamma2D" -variable spectrumType -value g2 -command SpectrumType2G -background $optionscolor
			radiobutton $type.g.gp -text "GammaP" -variable spectrumType -value gp -command SpectrumTypeGP -background $optionscolor
			pack $type.label
			pack $type.n.1d $type.n.2d $type.n.sum $type.n.bit -anchor w
			pack $type.n -side left
			pack $type.g.1g $type.g.2g $type.g.gp -anchor w
			pack $type.g -side left
		pack $type -expand 1 -fill both -side left
	
		set datatype $options.datatype
		frame $datatype -borderwidth 2 -relief groove -background $optionscolor
			label $datatype.label -text "Data type" -background $optionscolor
			radiobutton $datatype.byte -text "Byte (8 bits)" -variable spectrumDatatype -value byte -background $optionscolor
			radiobutton $datatype.word -text "Word (16 bits)" -variable spectrumDatatype -value word -background $optionscolor
			radiobutton $datatype.long -text "Long (32 bits)" -variable spectrumDatatype -value long -background $optionscolor
			pack $datatype.label
			pack $datatype.byte $datatype.word $datatype.long -anchor w
		pack $datatype -expand 1 -fill both -side left
		
		set control $options.control
		frame $control -borderwidth 2 -relief groove -background $optionscolor
			label $control.label -text "Definition file"  -background $optionscolor
			frame $control.buttons
				button $control.buttons.load -text "Load" -command LoadSpectrumDefinition  -background $optionscolor
				button $control.buttons.save -text "Save" -command SaveSpectrumDefinition  -background $optionscolor
				pack $control.buttons.load $control.buttons.save -side left
			label $control.file -textvariable definitionFile  -background $optionscolor
			checkbutton $control.delete -text "Cumulate" -variable deleteAll -background $optionscolor
			pack $control.label $control.file $control.buttons $control.delete
		pack $control -expand 1 -fill both -side right

	pack $options -expand 1 -fill both
	
	set middle $tops.middle
	set middlecolor lightyellow
	frame $middle -background $middlecolor

		set command $middle.command
		frame $command -borderwidth 2 -relief groove -background $middlecolor
			label $command.label -text "Spectrum name" -background $middlecolor
			entry $command.entry -width 20 -textvariable spectrumName -background $middlecolor
			button $command.generate -text "Create" -command GenerateSpectrum -background $middlecolor
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
			label $varx.varlabel -textvariable spectrumParameterX -background $bottomcolor
			menubutton $varx.varmenu -text "X Parameter" -background $bottomcolor
			label $varx.infolabel -textvariable spectrumInfoX -background $bottomcolor
			label $varx.reslabel -textvariable spectrumResolutionX -background $bottomcolor
			menubutton $varx.resmenu -text "Resolution" -background $bottomcolor
			grid $varx.varmenu $varx.resmenu  $varx.reslabel -sticky news
			grid $varx.varlabel $varx.infolabel x -sticky news
		pack $varx -expand 1 -fill both -side left
		
		set vary $bottom.vary
		frame $vary -borderwidth 2 -relief groove -background $bottomcolor
			label $vary.varlabel -textvariable spectrumParameterY -background $bottomcolor
			menubutton $vary.varmenu -text "Y Parameter" -background $bottomcolor
			label $vary.infolabel -textvariable spectrumInfoY -background $bottomcolor
			label $vary.reslabel -textvariable spectrumResolutionY -background $bottomcolor
			menubutton $vary.resmenu -text "Resolution" -background $bottomcolor
			grid $vary.varmenu $vary.resmenu  $vary.reslabel -sticky news
			grid $vary.varlabel $vary.infolabel x -sticky news
		pack $vary -expand 1 -fill both -side right
	
	pack $bottom -expand 1 -fill both
	
	set slist $tops.slist
	set slistcolor lightgreen
	frame $slist -borderwidth 2 -relief sunken -background $slistcolor
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
	
	set display $tops.display
	set displaycolor lightblue
	frame $display -borderwidth 2 -background $displaycolor -relief groove
#		radiobutton $display.activity -text Activity -variable ActivityMonitor -state disable \
		-width 8
#		label $display.text -text "Command: " -background $displaycolor -width 8
#		label $display.command -textvariable CommandText -background $displaycolor -width 20
		button $display.update -text "Update Spectrum List" -command UpdateSpectrumList -width 10
#		pack $display.activity -side left
		pack $display.update -side left -expand 1 -fill x
	pack $display -expand 1 -fill x

	pack $tops -expand 1 -fill both
}

proc SpectrumType1D {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state disabled
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text Parameter
	set wname $tops.bottom.vary
	$wname.varmenu configure -state disabled
	$wname.resmenu configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state normal
}

proc SpectrumType2D {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state normal
	$wname.long configure -state disabled
	set spectrumDatatype word
	$tops.bottom.varx.varmenu configure -text "X Parameter"
	set wname $tops.bottom.vary
	$wname.varmenu configure -state normal -text "Y Parameter"
	$wname.resmenu configure -state normal
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeSummary {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state normal
	$wname.long configure -state disabled
	set spectrumDatatype word
	set wname $tops.bottom.vary
	$tops.bottom.varx.varmenu configure -text Parameters
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
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text Parameter
	set wname $tops.bottom.vary
	$wname.varmenu configure -state disabled
	$wname.resmenu configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumType1G {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state disabled
	$wname.long configure -state normal
	set spectrumDatatype long
	$tops.bottom.varx.varmenu configure -text Parameters
	set wname $tops.bottom.vary
	$wname.varmenu configure -state disabled
	$wname.resmenu configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumType2G {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state normal
	$wname.long configure -state disabled
	set spectrumDatatype word
	$tops.bottom.varx.varmenu configure -text Parameters
	set wname $tops.bottom.vary
	$wname.varmenu configure -state disabled
	$wname.resmenu configure -state disabled
	set wname $tops.middle.command.generatearray
	$wname configure -state disabled
}

proc SpectrumTypeGP {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global tops
	set wname $tops.options.datatype
	$wname.byte configure -state normal
	$wname.long configure -state disabled
	set spectrumDatatype word
	$tops.bottom.varx.varmenu configure -text "X Parameter"
	set wname $tops.bottom.vary
	$wname.varmenu configure -state normal -text "Y Parameters"
	$wname.resmenu configure -state normal
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
	SendMessage "spectrum -list"
	set theList [GetResponse]
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
		g1 {
			set arrayname [string range $spectrumParameterX 0 [expr [string last . $spectrumParameterX] - 1]]
			foreach e $treeParameterName($arrayname) {
				if {[info exist treeParameterName($arrayname.$e)] == 0} {
					append spectrumParameterList "$arrayname.$e "
				}
			}
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionX
		}
		g2 {
			set arrayname [string range $spectrumParameterX 0 [expr [string last . $spectrumParameterX] - 1]]
			foreach e $treeParameterName($arrayname) {
				if {[info exist treeParameterName($arrayname.$e)] == 0} {
					append spectrumParameterList "$arrayname.$e "
				}
			}
			append spectrumResolutionList $spectrumResolutionX " " $spectrumResolutionX
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
		gp {
			if {[string match $spectrumParameterY ""] && $spectrumType == gp} {
				tk_messageBox -icon error -message "Please select the Y parameters" -title Error
				return
			}
			append spectrumParameterList $spectrumParameterX " "
			set arrayname [string range $spectrumParameterY 0 [expr [string last . $spectrumParameterY] - 1]]
			foreach e $treeParameterName($arrayname) {
				if {[info exist treeParameterName($arrayname.$e)] == 0} {
					append spectrumParameterList "$arrayname.$e "
				}
			}
			append spectrumResolutionList $spectrumResolutionX " " $spectrumResolutionY
			CreateSpectrum $spectrumList $spectrumName $spectrumParameterList $spectrumResolutionList
		}
	}
	UpdateSpectrumDisplay $spectrumName
	SendMessage "sbind -all"
	GetResponse
}

proc CreateSpectrum {SpectrumList Name ParameterList ResolutionList} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY spectrumArray
	
	puts "spectrum $Name $spectrumType {$ParameterList} {$ResolutionList} $spectrumDatatype"
	if {[lsearch $SpectrumList $Name] == -1} {
		SendMessage \
		"spectrum $Name $spectrumType {$ParameterList} {$ResolutionList} $spectrumDatatype"
		GetResponse
		SendMessage "puts \"Spectrum $Name created\""
		GetResponse
	} else {
		if {[string match [tk_messageBox -icon warning -message "The spectrum $Name already exists.  Do you want to overwrite it?" \
		-title Warning -type yesno] yes]} {
			SendMessage "spectrum -delete $Name"
			GetResponse
			SendMessage \
			"spectrum $Name $spectrumType {$ParameterList} {$ResolutionList} $spectrumDatatype"
			GetResponse
			SendMessage "puts \"Spectrum $Name replaced\""
			GetResponse
		}
	}
	ModifiedSpectrumDefinition
}

proc UpdateSpectrumDisplay {spectrum} {
	global tops
	SendMessage "spectrum -list $spectrum"
	set theSpectrum [GetResponse]
	if {[string compare $theSpectrum "Spectrum does not exist"] == 0} {return}
	SendMessage "apply -list $spectrum"
	set theApply [GetResponse]
	set id [lindex $theSpectrum 0]
	set name [lindex $theSpectrum 1]
	set type [lindex $theSpectrum 2]
	set parameters [lindex $theSpectrum 3]
	set resolutions [lindex $theSpectrum 4]
	set data [lindex $theSpectrum 5]
	set gate [lindex [lindex [lindex $theApply 0] 1] 0]
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
		g1 {
			set ltype "G1"
			set varx [lindex $parameters 0]
			set resx $resolutions
			set vary ""
			set resy ""
		}
		g2 {
			set ltype "G2"
			set varx [lindex $parameters 0]
			set resx [lindex $resolutions 0]
			set vary ""
			set resy ""
		}
		gp {
			set ltype "GP"
			set varx [lindex $parameters 0]
			set resx [lindex $resolutions 0]
			set vary [lindex $parameters 1]
			set resy [lindex $resolutions 1]
		}
		default {
		}
	}
	set index [lsearch [$tops.slist.listbox.framename.listbox get 0 end] $spectrum]
	$tops.slist.listbox delete $index $index
	$tops.slist.listbox insert $index [list $name $ltype $data $varx $resx $vary $resy $gate]
	sort $tops.slist.listbox name
	set index [lsearch [$tops.slist.listbox.framename.listbox get 0 end] $spectrum]
	$tops.slist.listbox see $index
}

proc UpdateSpectrumList {} {
	global tops
	set fraction [lindex [$tops.slist.listbox yview] 0]
	$tops.slist.listbox delete 0 end
	SendMessage "spectrum -list"
	set theList [GetResponse]
	SendMessage "apply -list"
	set theApplyList [GetResponse]
	set i 0
	foreach spectrum $theList {
		set id [lindex $spectrum 0]
		set name [lindex $spectrum 1]
		set type [lindex $spectrum 2]
		set parameters [lindex $spectrum 3]
		set resolutions [lindex $spectrum 4]
		set data [lindex $spectrum 5]
		set appliedGate [lindex $theApplyList $i]
		set gate [lindex [lindex $appliedGate 1] 0]
		if {[string match $gate -TRUE-]} {set gate ""}
		if {[string match $gate -Ungated-]} {set gate ""}
		incr i
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
			g1 {
				set ltype "G1"
				set varx [lindex $parameters 0]
				set resx $resolutions
				set vary ""
				set resy ""
			}
			g2 {
				set ltype "G2"
				set varx [lindex $parameters 0]
				set resx [lindex $resolutions 0]
				set vary ""
				set resy ""
			}
			gp {
				set ltype "GP"
				set varx [lindex $parameters 0]
				set resx [lindex $resolutions 0]
				set vary [lindex $parameters 1]
				set resy [lindex $resolutions 1]
			}
		    default {
		    }
		}
		$tops.slist.listbox insert end [list $name $ltype $data $varx $resx $vary $resy $gate]
	}
	$tops.slist.listbox yview moveto $fraction
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
		G1 {set type g1; SpectrumType1G}
		G2 {set type g2; SpectrumType2G}
		GP {set type gp; SpectrumTypeGP}
	}
	set spectrumType $type
	set spectrumDatatype [lindex $spectrum 2]
	set spectrumParameterX [lindex $spectrum 3]
	set spectrumResolutionX [lindex $spectrum 4]
	CreateResolutionMenu $tops.bottom.varx [GetParameterResolution $spectrumParameterX] spectrumResolutionX
	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
	if {[string compare $type 2] == 0 || [string compare $type gp] == 0} {
		set spectrumParameterY [lindex $spectrum 5]
		set spectrumResolutionY [lindex $spectrum 6]
		CreateResolutionMenu $tops.bottom.vary [GetParameterResolution $spectrumParameterY] spectrumResolutionY
		set spectrumInfoY [GetParameterInfo $spectrumParameterY]
	}
	set spectrumGate [lindex $spectrum 7]
}

proc DeleteSpectra {} {
	global tops clearAll
	
	if {$clearAll} {
		if {[string compare no [tk_messageBox -message \
		"Are you sure you want to delete all spectra?" -type yesno]] == 0} {return}
		SendMessage "spectrum -delete -all"
		GetResponse
		return
	}
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	foreach index $selectedlist {
		set spectrum [$lbox get $index]
		SendMessage "spectrum -delete [lindex $spectrum 0]"
		GetResponse
		$lbox delete $index $index
	}
	ModifiedSpectrumDefinition
}

proc ClearSpectra {} {
	global tops clearAll
	
	if {$clearAll} {
		SendMessage "clear -all"
		GetResponse
		return
	}
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	foreach index $selectedlist {
		set spectrum [$lbox get $index]
		SendMessage "clear [lindex $spectrum 0]"
		GetResponse
	}
}

proc DuplicateSpectra {} {
	global tops
	
	set lbox $tops.slist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
        SendMessage "spectrum -list"
	set spectrumList [GetResponse]
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
	        SendMessage "spectrum -list $spectrumName"
		set newSpectrum [GetResponse]
		set type [lindex $newSpectrum 2]
		set parameters [lindex $newSpectrum 3]
		set resolutions [lindex $newSpectrum 4]
		set datatype [lindex $newSpectrum 5]
	       SendMessage "spectrum $try $type {$parameters} {$resolutions} $datatype"
	       GetResponse
	       $lbox insert [expr $index+1] [list $try "" "" "" "" "" "" ""]
	       UpdateSpectrumDisplay $try
	}
#	UpdateSpectrumList
	SendMessage "sbind -all"
	GetResponse
	ModifiedSpectrumDefinition
}

proc ModifiedSpectrumDefinition {} {
	global definitionFile
	if {[string compare [lindex $definitionFile 1] (modified)] != 0} {
		set definitionFile "$definitionFile (modified)"
	}
}

proc SaveSpectrumDefinition {} {
	global definitionFile fullDefinitionFile definitionDirectory
	
	if {[string compare $definitionDirectory ""] == 0} {set definitionDirectory [pwd]}
	set fullDefinitionFile [tk_getSaveFile -filetypes {\
	{"Tcl script file" {.tcl}}\
	{"Definition file" {.sdef}}\
	}\
	-initialdir $definitionDirectory -initialfile [lindex $definitionFile 0] \
	-defaultextension .tcl \
	-title "Save Spectrum Definition file"]
	if {[string match $fullDefinitionFile ""]} {return}
	set definitionFile [lindex [split $fullDefinitionFile /] end]
	set definitionDirectory [string trimright $fullDefinitionFile $definitionFile]

	SendMessage "SaveSpectrumDefinition $fullDefinitionFile"
	GetResponse
}

proc LoadSpectrumDefinition {} {
	global spectrumType spectrumDatatype spectrumName spectrumParameterX spectrumResolutionX spectrumParameterY spectrumResolutionY
	global spectrumInfoX spectrumInfoY
	global definitionFile fullDefinitionFile definitionDirectory deleteAll
	
	set fullDefinitionFile [tk_getOpenFile -filetypes {\
	{"Tcl Script file" {.tcl}}\
	{"Definition file" {.sdef}}\
	}\
	-initialdir $definitionDirectory -title "Load Spectrum Definition file"]
	if {[string match $fullDefinitionFile ""]} {return}
	set definitionFile [lindex [split $fullDefinitionFile /] end]
	set definitionDirectory [string trimright $fullDefinitionFile $definitionFile]
	if {$deleteAll == 0} {
		SendMessage "spectrum -delete -all"
		GetResponse
	}
	SendMessage "LoadSpectrumDefinition $fullDefinitionFile"
	GetResponse
	UpdateTreeParameters
	set spectrumInfoX [GetParameterInfo $spectrumParameterX]
	set spectrumInfoY [GetParameterInfo $spectrumParameterY]
	UpdateGateList
	UpdateSpectrumList
}

proc GenerateGateMenu {parent command} {
# Generate a menu containing all available gates and attaches it to the parent widget
	
	destroy $parent.menu
	menu "$parent.menu" -tearoff 0
	SendMessage "gate -list"
	set theList [GetResponse]
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
		SendMessage "apply $spectrumGate $name"
		GetResponse
		UpdateSpectrumDisplay $name
	}
	ModifiedSpectrumDefinition
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
		SendMessage "catch {ungate $name}"
		GetResponse
		UpdateSpectrumDisplay $name
	}
	ModifiedSpectrumDefinition
}
