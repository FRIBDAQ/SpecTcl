
#   Procedure to format a set of parameter definition lists
#   into a nice output string which can be used as desired.
#   Typical use might be:
#       
#     ParList { [parameter -list -byid] }
#
proc ParList { ParameterList } {
    set output "Name\t\tId\t\tResolution\n"
    set fmt    "%s \t %d  \t\t %d \n"
    foreach Parameter $ParameterList {
	set line [format $fmt [lindex $Parameter 0] [lindex $Parameter 1] [lindex $Parameter 2]]
	append output $line

    }
    return $output
}
proc SaveParams { file ParameterList } {
    set fmt "parameter %s %d %d\n"
    foreach Parameter $ParameterList {
	puts $file [format $fmt [lindex $Parameter 0] [lindex $Parameter 1] [lindex $Parameter 2]]
    }	
}

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



