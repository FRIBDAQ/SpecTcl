#
#
#  Setup the standard scripted commands in SpecTcl.
#

tk appname SpecTcl

puts -nonewline "Loading SpecTcl GUI..."
source ~/SpecTcl/contrib/treeparam/SpecTclGui.tcl
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
