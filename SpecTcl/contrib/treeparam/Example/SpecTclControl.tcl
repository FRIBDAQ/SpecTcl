wm title . "S800 SpecTcl Control"

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
	exec netscape $URL &
}

proc AttachOnline {} {
	global SpecTclHome
	global RunState
	global DataSource
	global DAQHOST
	if $RunState {
		stop
	}
	attach -pipe /opt/daq/Bin/spectcldaq "TCP://$DAQHOST:2602/"
	set DataSource "TCP://$DAQHOST:2602/"
	after 100 start
}

proc AttachFile {} {
	global SpecTclHome
	global RunState
	global DataSource
	global EventDirectory
	if {[string compare $EventDirectory ""] == 0} {set EventDirectory /user/s800/event}
	set fname [tk_getOpenFile -filetypes {{"Event files" {.evt}}} -initialdir \
		 $EventDirectory -title "Select Listmode File:"]
	if {$fname != {} } {
		if $RunState {
			stop
		}
		set eventFile [lindex [split $fname /] end]
		set EventDirectory [string trimright $fname $eventFile]
		attach -file $fname
		set DataSource $fname
		after 100 start
	}
}

proc AttachFiles {} {
	global SpecTclHome
	global RunState
	global DataSource
	global EventDirectory
	if {[string compare $EventDirectory ""] == 0} {set EventDirectory /user/s800/event}
	set fname [tk_getOpenFile -filetypes {{"Cluster files" {.clu}}} -initialdir \
		 $EventDirectory -title "Select Cluster File:"]
	if {$fname != {} } {
		if $RunState {
			stop
		}
		set file [open $fname r]
		while {![eof $file]} {
			append fileList "[gets $file] "
		}
		close $file
		set eventFile [lindex [split $fname /] end]
		set EventDirectory [string trimright $fname $eventFile]
		attach -pipe cat $fileList
		set DataSource $fname
		after 100 start
	}
}

proc LoadS800Map {} {
	global map
	global s800.fp.track.map.maxorder
	global s800.fp.track.order
	if {[info exists s800.fp.track.map.maxorder] == 0} {
		set str "This version of SpecTcl\n"
		append str "doesn't support inverse maps.\n"
		tk_messageBox -message $str -icon error
		return
	}
	set map(filename) [tk_getOpenFile -filetypes {{"Inverse maps" {.inv}}} \
		-title "Select S800 inverse map:"]
	if {$map(filename) != {}} {
		ParseS800Map
		set s800.fp.track.order $map(order)
	}
}

# This procedure parses a COSY map into the CS800Map class using s800map command
proc ParseS800Map {} {
	global map
	global s800.fp.track.map.maxorder 
	global s800.fp.track.map.maxcoefficients
	global s800.fp.track.map.maxparameters
	global s800.fp.track.order
	set map(title) ""
	set file [open $map(filename) "r"]
	set line [gets $file]
	while {![string match *COEFFICIENT* $line]} {
		append map(title) "$line\n"
		set line [gets $file]
	}
	set parameter 0
	while {![eof $file]} {
		set line [gets $file]
		while {![string match *-----------------* $line]} {
			scan $line "%d %g %d %d %d %d %d %d %d" \
			index coefficient order exp(0) exp(1) exp(2) exp(3) exp(4) exp(5)
			if {$order > ${s800.fp.track.map.maxorder}} {
				set s800.fp.track.map.maxorder $order
				set map(order) $order
				$map(menu) add radiobutton -label $order -variable map(order) \
				-command "set s800.fp.track.order $map(order)"
			}
			if {$index > ${s800.fp.track.map.maxcoefficients}} {
				set str "Error: too many coefficients in map\n"
				append str "Please increase CS800_TRACK_COEFFICIENTS\n"
				append str "and recompile SpecTcl."
				tk_messageBox -message $str -icon error
				break
			}
			s800map -set $parameter $order $index $exp(0) $exp(1) $exp(2) \
			$exp(3) $exp(4) $exp(5) $coefficient
			set line [gets $file]
		}
		set line [gets $file]
		incr parameter
		if {$parameter > ${s800.fp.track.map.maxparameters}} {
			set str "Error: too many parameters in map\n"
			append str "Please increase CS800_TRACK_PARAMETERS\n"
			append str "and recompile SpecTcl."
			tk_messageBox -message $str -icon error
			break
		}
	}
	close $file
}

frame .m -relief groove -borderwidth 2

frame .m.top
frame .m.top.tl
button .m.top.tl.startstop -textvariable StartButtonText -command StartStop -width 15
button .m.top.tl.clearall  -text "Clear Spectra" -command {clear -all} -width 15
button .m.top.tl.help -text "Help" -command {Help} -width 15
button .m.top.tl.exit -text Exit -command Exit -width 15
pack .m.top.tl.startstop -side top -fill x
pack .m.top.tl.clearall -side top -fill x
pack .m.top.tl.help -side top -fill x
pack .m.top.tl.exit -side top -fill x

image create photo s800image -file "s800.gif" -format gif

frame .m.top.tm -borderwidth 1
label .m.top.tm.s800 -image s800image -relief groove -borderwidth 2
pack .m.top.tm.s800 -fill x -expand true -fill y 

pack .m.top.tl -side left -fill x
pack .m.top.tm -fill x -expand true -fill y 

frame .m.bot
button .m.bot.attonl -text "Attach Online" -command AttachOnline
button .m.bot.attfile -text "Attach to File" -command AttachFile
button .m.bot.attfiles -text "Attach to Files" -command AttachFiles
pack .m.bot.attonl -side left -fill x -expand true
pack .m.bot.attfile -side right -fill x -expand true
pack .m.bot.attfiles -side right -fill x -expand true

frame .m.stat -relief groove -borderwidth 2
frame .m.stat.buf 
label .m.stat.buf.head -text "Analyzed Buffers: "
label .m.stat.buf.val -textvariable BuffersAnalyzed
pack .m.stat.buf.head .m.stat.buf.val -side left

frame .m.stat.runno
label .m.stat.runno.head -text "Run Number: "
label .m.stat.runno.val -textvariable RunNumber
pack .m.stat.runno.head .m.stat.runno.val -side left

frame .m.stat.source
label .m.stat.source.head -text "Source: "
label .m.stat.source.val -textvariable DataSource
pack .m.stat.source.head .m.stat.source.val -side left

pack .m.stat.source .m.stat.runno .m.stat.buf -side top -fill x

frame .m.map -relief groove -borderwidth 2
frame .m.map.file
label .m.map.file.head -text "Inverse Map: "
label .m.map.file.val -textvariable map(filename)
pack .m.map.file.head .m.map.file.val -side left

frame .m.map.title
label .m.map.title.head -text "Title: "
label .m.map.title.val -textvariable map(title)
pack .m.map.title.head .m.map.title.val -side left

frame .m.map.order
label .m.map.order.head -text "Order: "
set map(menu) [tk_optionMenu .m.map.order.val map(order) 0]
button .m.map.order.loadmap -text "Load Inverse Map" -command LoadS800Map
pack .m.map.order.head .m.map.order.val -side left
pack .m.map.order.loadmap -side right

pack .m.map.file .m.map.title .m.map.order -side top -fill x

pack .m.top  -side top -fill x
pack .m.bot  -side bottom -fill x
pack .m.stat -side bottom -fill x
pack .m.map -side bottom -fill x
pack .m

trace variable RunState w  UpdateStartButton

proc cntrlupdate {} {
    after 1000 cntrlupdate
    update idle
}
cntrlupdate


