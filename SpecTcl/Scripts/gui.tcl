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

button .exit -text Exit -command "exit"
button .startstop -textvariable StartButtonText -command StartStop
button .clearall  -text "Clear Spectra" -command {clear -all}
button .help      -text "Help"          -command {Help}
label   .speclbl  -text "Defined Spectra"
listbox .spectra

pack .startstop .clearall .exit .help .speclbl .spectra  -side top -fill x

trace variable RunState w  UpdateStartButton

proc specupdate {} {
    .spectra delete 0 [.spectra size]
    foreach spec [spectrum -list] {
      .spectra insert end [lindex $spec 1]
    }
    after 1000 specupdate
    update idle
}
specupdate
