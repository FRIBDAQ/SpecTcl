#
#   SpecTcl extension to support the automated export of a 1-d spectrum 
#  to xmgrace.
#
#  To use:
#     source $SpecTclHome/contrib/Grace/grace.tcl
#     pack .grace  #however.
#
# $Log$
# Revision 5.1  2004/11/29 16:56:21  ron-fox
# Begin port to 3.x compilers calling this 3.0
#
# Revision 1.1  2003/09/03 17:35:05  ron-fox
# Added new contributed item:  $SpecTclHome/contrib/grace.tcl exports 1-d spectra to xmgrace Makefile.am updated to install this.
# grace.tcl - script to setup the interface:
#     pack [source $SpecTclHome/contrib/Grace/grace.tcl]
#     click on the Export Grace... button, choose a spectrum.
# sfilter.tcl - Script to filter the output of swrite to grace readable format.
#
#
proc Grace {spectrum} {
    global SpecTclHome
  set fd [open "| $SpecTclHome/contrib/Grace/sfilter.tcl > /tmp/grace.dat" w]
  swrite -format ascii $fd $spectrum
  close $fd
  exec xmgrace /tmp/grace.dat &
}

proc GraceIt {listwidget toplevel} {
    set selectedidx [$listwidget curselection]
    if {$selectedidx != ""} { 
	set spectrum [$listwidget get $selectedidx]
	if {$spectrum != ""} {
	    Grace $spectrum
	}
    }
    destroy $toplevel
}
proc GraceSelect {} {
    set spectra [spectrum -list]
    set names ""
    foreach spectrum $spectra {
	set type [lindex $spectrum 2]
	if {($type == "1") || ($type == "b") ||
	($type == "g1")} {
	    lappend names [lindex $spectrum 1]
	}
    }
    if {$names == "" } {
	tk_dialog .info "No Spectra" "No 1-d spectra are defined" \
	info 0 "Dismiss"
    } else {
	toplevel .dialog
	set la [frame .dialog.listarea]
	set ba [frame .dialog.buttonarea]
	listbox $la.spectra -yscrollcommand "$la.scroll set"
	scrollbar $la.scroll -orient vertical -command "$la.spectra yview"
	
	foreach name $names {
	    $la.spectra insert end $name
	}
	
	
	set OkCommand "GraceIt $la.spectra .dialog"
	
	button  $ba.cancel -text Cancel -command "destroy .dialog"
	button  $ba.ok     -text Ok -command $OkCommand
	bind    $la.spectra <Double-1> $OkCommand
	
	pack $la $ba -side top -fill x
	pack $la.spectra -fill x -side left
	pack $la.scroll -fill y -side right
	
	pack $ba.ok $ba.cancel -side left
	
    }
}
destroy .grace
button .grace -text "Export Grace..." -command "GraceSelect"
# pack .grace
