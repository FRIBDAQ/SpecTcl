#
#
#  Setup the standard scripted commandsin SpecTcl.
#

tk appname SpecTcl

puts -nonewline "Loading SpecTcl gui..."
source $SpecTclHome/Script/gui.tcl
puts  "Done."

puts -nonewline "Loading state I/O scripts..."
source $SpecTclHome/Script/fileall.tcl
puts "Done."

puts -nonewline "Loading formatted listing scripts..."
source $SpecTclHome/Script/listall.tcl
puts "Done."

puts -nonewline "Loading gate copy script procs..."
source $SpecTclHome/Script/CopyGates.tcl
puts "Done."

puts -nonewline "Loading my SpecTcl GUI..."
source SpecTclGui.tcl
puts "Done."

proc att {} {
    global SpecTclHome
    global RunState
    if $RunState stop
    attach -pipe $SpecTclHome/Bin/spectcldaq TCP://spdaq06.nscl.msu.edu:2602/ 
    after 100 start
}

proc att_file {} {
    global SpecTclHome
    global RunState
    set fname [tk_getOpenFile -defaultextension ".evt" -initialdir "/usr/DAQ/event1" -title "Select Listmode File:"]
    if {$fname != {} } {
	if $RunState stop
	attach -file $fname
	after 100 start
    }
}

button .attonl -text "Attach Online" -command att
button .attfile -text "Attach to File" -command att_file
pack .attonl .attfile
