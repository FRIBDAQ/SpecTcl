# GateGenerator.tcl
# Author: D. Bazin
# Date: September 2002
# Version 1.2: November 2003

proc SetupGateGenerator {parent} {
	global topg
	CreateGateGenerator $parent
	UpdateGateList
}

proc CreateGateGenerator {parent} {
	global topg GateName GateDependency
	set topg [tabnotebook_page $parent Gates]
	
	set wname $topg.create
	frame $wname -borderwidth 2 -relief groove -background lightyellow
		frame $wname.l -borderwidth 2 -background lightyellow
		frame $wname.r -borderwidth 2 -background lightyellow
		frame $wname.r.t
		frame $wname.r.b
#		frame $wname.c
		button $wname.l.create -text Create/Replace -command CreateGate -background lightyellow
		entry $wname.l.name -width 15 -textvariable GateName -background  lightyellow
		menubutton $wname.r.t.gate -text "Gate select" -background lightyellow
		tk_optionMenu $wname.r.t.operation GateOperation And Or Not C2Band Slice Contour Band \
		GammaBand GammaContour GammaSlice
		$wname.r.t.operation configure -background lightyellow
		entry $wname.r.b.depend -width 80 -textvariable GateDependency -background lightyellow
		button $wname.r.t.clear -text "Clear dependency" -background lightyellow -command {set GateDependency ""}
#		button $wname.c.delete -text "Delete Selected" -command DeleteGate -width 15 -background lightblue
#		button $wname.c.deleteall -text "Delete All" -command DeleteGateAll -width 15 -background lightblue
		pack $wname.l.create $wname.l.name -expand 1 -fill x
		pack $wname.r.t.gate $wname.r.t.clear $wname.r.t.operation -expand 1 -fill x -side left
		pack $wname.r.b.depend -expand 1 -fill x
#		pack $wname.c.delete $wname.c.deleteall -expand 1 -fill x -side top
		pack $wname.r.t $wname.r.b -side top -expand 1 -fill x
		pack $wname.l $wname.r -expand 1 -fill x -side left
	pack $wname -expand 1 -fill x -anchor n
	bind $wname.r.t.gate <ButtonPress-1> "GenerateGateMenu $wname.r.t.gate GateGateCommand"

	set wname $topg.commands
	frame $wname -borderwidth 2 -relief groove -background lightblue
		button $wname.delete -text "Delete Selected" -command DeleteGate -background lightblue
		button $wname.deleteall -text "Delete All" -command DeleteGateAll -background lightblue
		pack $wname.delete $wname.deleteall -expand 1 -fill x -side left
	pack $wname -expand 1 -fill x -anchor n
	
	set glist $topg.glist
	set glistcolor lightgreen
	frame $glist -borderwidth 2 -relief sunken -background $glistcolor
	scrollbar $glist.vsb -orient vertical -command [list $glist.listbox yview] -background $glistcolor
	scrollbar $glist.hsb -orient horizontal -command [list $glist.listbox xview] -background $glistcolor
	set lbox $glist.listbox
	mclistbox::mclistbox $lbox -fillcolumn name -height 25 -width 60 \
	-labelanchor w -selectmode extended -labelborderwidth 2 \
	-xscrollcommand [list $glist.hsb set] -yscrollcommand [list $glist.vsb set] -background $glistcolor
	$lbox column add name -label Name -width 15
	$lbox column add type -label Type -width 15
	$lbox column add depend -label Dependency -width 60
	$lbox label bind name <ButtonPress-1> "sort %W name"
	$lbox label bind type <ButtonPress-1> "sort %W type"
	$lbox label bind depend <ButtonPress-1> "sort %W depend"
	bind $lbox <Double-ButtonPress-1> FillGateEntries
	grid $glist.vsb -in $glist -row 0 -column 1 -sticky ns
	grid $glist.hsb -in $glist -row 1 -column 0 -sticky ew
	grid $lbox -in $glist -row 0 -column 0 -sticky news -padx 0 -pady 0
	grid columnconfigure $glist 0 -weight 1
	grid columnconfigure $glist 1 -weight 0
	grid rowconfigure $glist 0 -weight 1
	grid rowconfigure $glist 1 -weight 0
	pack $glist -fill both -expand 1		

	set display $topg.display
	set displaycolor lightblue
	frame $display -borderwidth 2 -background $displaycolor -relief groove
		button $display.update -text "Update Gate List" -command UpdateGateList
		pack $display.update -side left -expand 1 -fill x
	pack $display -expand 1 -fill x

	pack $topg -expand 1 -fill both -anchor n
}

proc GateGateCommand {theGate} {
	global topg
	if {[llength $theGate] > 1} {
		set theGate "{$theGate}"
	}
	$topg.create.r.b.depend insert end "$theGate "
}

proc UpdateGateList {} {
	global topg
	$topg.glist.listbox delete 0 end
	set theList [gate -list]
	foreach gate $theList {
		set name [lindex $gate 0]
		set id [lindex $gate 1]
		set type [lindex $gate 2]
		switch -- $type {
			s {set type Slice}
			c {set type Contour}
			* {set type And}
			+ {set type Or}
			- {set type Not}
			c2band {set type C2Band}
			b {set type Band}
			gb {set type GammaBand}
			gc {set type GammaContour}
			gs {set type GammaSlice}
		}
		set depend [lindex $gate 3]
		if {[string compare $type F] != 0 && [string compare $type T] != 0} {
			$topg.glist.listbox insert end [list $name $type $depend]
		}
	}
}

proc DeleteGate {} {
	global topg
	set lbox $topg.glist.listbox
	set selectedlist [$lbox curselection]
	if {[string match $selectedlist ""]} {return}
	foreach index $selectedlist {
		set gate [$lbox get $index]
		gate -delete [lindex $gate 0]
	}
	UpdateGateList
	Modified
}

proc DeleteGateAll {} {
	global topg
	if {[string compare no [tk_messageBox -message \
	"Are you sure you want to delete all gates?" -type yesno]] == 0} {return}
	set lbox $topg.glist.listbox
	foreach g [$lbox.framename.listbox get 0 end] {
		if {[llength $g] > 1} {
			set g "{$g}"
		}
		gate -delete $g
	}
#	gate -delete [$lbox.framename.listbox get 0 end]
	UpdateGateList
	Modified
}

proc FillGateEntries {} {
	global topg GateName GateDependency
	set lbox $topg.glist.listbox
	set index [lindex [$lbox curselection] 0]
	set gate [$lbox get $index]
	set GateName [lindex $gate 0]
	set type [lindex $gate 1]
	set GateDependency [lindex $gate 2]
	switch -- $type {
		And {$topg.create.r.t.operation.menu invoke 0}
		Or {$topg.create.r.t.operation.menu invoke 1}
		Not {$topg.create.r.t.operation.menu invoke 2}
		C2Band {$topg.create.r.t.operation.menu invoke 3}
		Slice {$topg.create.r.t.operation.menu invoke 4}
		Contour {$topg.create.r.t.operation.menu invoke 5}
		Band {$topg.create.r.t.operation.menu invoke 6}
		GammaBand {$topg.create.r.t.operation.menu invoke 7}
		GammaContour {$topg.create.r.t.operation.menu invoke 8}
		GammaSlice {$topg.create.r.t.operation.menu invoke 9}
		default {$topg.create.r.t.operation.menu invoke 0}
	}
}

proc CreateGate {} {
	global topg GateName GateDependency GateOperation
	if {[string compare $GateName ""] == 0} {return}
	if {[llength $GateName] > 1} {
		set GateName "{$GateName}"
	}
	switch -- $GateOperation {
		And {
			if {[llength $GateDependency] < 2} {
				tk_messageBox -icon error -message "Please enter at least two gates in the dependency"
				return
			}
			gate $GateName * $GateDependency
		}
		Or {
			if {[llength $GateDependency] < 2} {
				tk_messageBox -icon error -message "Please enter at least two gates in the dependency"
				return
			}
			gate $GateName + $GateDependency
		}
		Not {
			if {[llength $GateDependency] != 1} {
				tk_messageBox -icon error -message "Please enter only one gate in the dependency"
				return
			}
			gate $GateName - $GateDependency
		}
		C2Band {
			if {[llength $GateDependency] != 2} {
				tk_messageBox -icon error -message "Please enter only two gates in the dependency"
				return
			}
			gate $GateName c2band $GateDependency
		}
		Slice {
			eval gate $GateName s "{$GateDependency}"
		}
		Contour {
			set lg [llength $GateDependency]
			set lp ""
			for {set i 1} {$i < $lg} {incr i} {
				lappend lp [lindex $GateDependency $i]
			}
			eval gate $GateName c "{[lindex [lindex $GateDependency 0] 0] [lindex [lindex $GateDependency 0] 1] {$lp}}"
		}
		Band {
			set lg [llength $GateDependency]
			set lp ""
			for {set i 1} {$i < $lg} {incr i} {
				lappend lp [lindex $GateDependency $i]
			}
			eval gate $GateName b "{[lindex [lindex $GateDependency 0] 0] [lindex [lindex $GateDependency 0] 1] {$lp}}"
		}
		GammaBand {
			set lg [llength $GateDependency]
			set lp ""
			for {set i 1} {$i < $lg} {incr i} {
				lappend lp [lindex $GateDependency $i]
			}
			eval gate $GateName gb "{[lindex [lindex $GateDependency 0] 0] [lindex [lindex $GateDependency 0] 1] {$lp}}"
		}
		GammaContour {
			set lg [llength $GateDependency]
			set lp ""
			for {set i 1} {$i < $lg} {incr i} {
				lappend lp [lindex $GateDependency $i]
			}
			eval gate $GateName gc "{[lindex [lindex $GateDependency 0] 0] [lindex [lindex $GateDependency 0] 1] {$lp}}"
		}
		GammaSlice {
			eval gate $GateName gs "{$GateDependency}"
		}
	}
	UpdateGateList
	Modified
}
	
