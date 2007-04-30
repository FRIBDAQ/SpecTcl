# $Id$
#


if {[catch {set DAQHOST $env(DAQHOST)} result]} {
  set DAQHOST "localhost"
}

set me     [info script]
set here   [file dirname $me]
lappend auto_path $here

set gifname $here/tunl-logo-seal.gif
source $here/CalibrateMainGui.tcl
package require CalibIOGUI


set StartButtonText  "Start Analysis"
proc StartStop {} {
	global RunState
	if { $RunState } {
		stop
	} else {
		start
	}
}

proc UpdateStartButton {name element op} {
	global RunState
	global StartButtonText
	if { $RunState } {
		set StartButtonText "Stop Analysis"
	} else {
		set StartButtonText "Start Analysis"
	}
}

proc Help {} {
	global SpecTclHome
	set URL [format "%s%s" $SpecTclHome /doc/index.htm]
	exec mozilla $URL &
}



proc AttachOnline {} {
	global SpecTclHome
	global RunState
	global DataSource
	global DAQHOST
        global env

	if $RunState {
		stop
	}
	set DataSource "TCP://$DAQHOST:2602/"
	attach -pipe /usr/opt/daq/Bin/spectcldaq $DataSource
	start
}

proc AttachFile {} {
	global SpecTclHome
	global RunState
	global DataSource
	set fname [tk_getOpenFile -defaultextension ".evt" -initialdir \
		 "~/stagearea/complete" -title "Select Listmode File:"]
	if {$fname != {} } {
	    if $RunState {
		stop
	    }
	    attach -file $fname
	    set DataSource $fname
	    start
	}
}

if {[info var paneltop] == ""} {
    set paneltop ""
}


frame $paneltop.m -relief groove -borderwidth 2

frame $paneltop.m.top
frame $paneltop.m.top.tl
button $paneltop.m.top.tl.startstop -textvariable StartButtonText -command StartStop -width 15
button $paneltop.m.top.tl.clearall  -text "Clear Spectra" -command {clear -all} -width 15
button $paneltop.m.top.tl.calibrate -text "Calibrate..."  -command CalibrationGUI::CalibrationGUI
button $paneltop.m.top.tl.savecalib -text "Save Calibrations..." \
    -command CalibIOGUI::WriteConfiguration
button $paneltop.m.top.tl.readcalib -text "Read Calibrations..." \
    -command CalibIOGUI::ReadConfiguration
pack $paneltop.m.top.tl.startstop \
     $paneltop.m.top.tl.calibrate \
     $paneltop.m.top.tl.savecalib \
     $paneltop.m.top.tl.readcalib \
                                    -side top -fill x
pack $paneltop.m.top.tl.clearall -side bottom -fill x

frame $paneltop.m.top.tr
button $paneltop.m.top.tr.help -text "Help" -command {Help} -width 12
button $paneltop.m.top.tr.exit -text Exit -command "exit" -width 12
pack $paneltop.m.top.tr.help -side top -fill x
pack $paneltop.m.top.tr.exit -side bottom -fill x


#
#   If you have a nice image  you'd like displayed in the middle of the
#   GUI, uncomment the three lines that begin with image below
#   substitute the filename for your gif in the image file.

frame $paneltop.m.top.tm -borderwidth 1

 image create photo logoimage -file $gifname -format gif
 label $paneltop.m.top.tm.a1900 -image logoimage -relief groove -borderwidth 2
 pack $paneltop.m.top.tm.a1900 -fill x -expand true -fill y 

pack $paneltop.m.top.tl -side left -fill x
pack $paneltop.m.top.tr -side right -fill x
pack $paneltop.m.top.tm -fill x -expand true -fill y 

frame $paneltop.m.bot
button $paneltop.m.bot.attonl -text "Attach Online" -command AttachOnline
button $paneltop.m.bot.attfile -text "Attach to File" -command AttachFile
pack $paneltop.m.bot.attonl -side left -fill x -expand true
pack $paneltop.m.bot.attfile -side right -fill x -expand true

frame $paneltop.m.stat -relief groove -borderwidth 2
frame $paneltop.m.stat.buf 
label $paneltop.m.stat.buf.head -text "Analysis percentage  : "
label $paneltop.m.stat.buf.val -textvariable Efficiency
label $paneltop.m.stat.buf.tothead -text " Buffers: "
label $paneltop.m.stat.buf.totals  -textvariable BuffersAnalyzed
pack $paneltop.m.stat.buf.head $paneltop.m.stat.buf.val \
    $paneltop.m.stat.buf.tothead $paneltop.m.stat.buf.totals \
    -side left

frame $paneltop.m.stat.runno
label $paneltop.m.stat.runno.head -text "Run Number: "
label $paneltop.m.stat.runno.val -textvariable RunNumber
pack $paneltop.m.stat.runno.head $paneltop.m.stat.runno.val -side left

frame $paneltop.m.stat.source
label $paneltop.m.stat.source.head -text "Source: "
label $paneltop.m.stat.source.val -textvariable DataSource
pack $paneltop.m.stat.source.head $paneltop.m.stat.source.val -side left



pack $paneltop.m.stat.source $paneltop.m.stat.runno $paneltop.m.stat.buf -side top -fill x

pack $paneltop.m.top  -side top -fill x
pack $paneltop.m.bot  -side bottom -fill x
pack $paneltop.m.stat -side bottom -fill x




pack $paneltop.m -expand false

wm resizable . 0 0

trace variable RunState w  UpdateStartButton

set Efficiency 0

proc cntrlupdate {} {
    global Efficiency
    global BuffersAnalyzed
    global LastSequence

    if {$LastSequence != 0} {
	set Efficiency [expr 100*($BuffersAnalyzed/$LastSequence)]
    }

    after 1000 cntrlupdate
    update idle
}
cntrlupdate


