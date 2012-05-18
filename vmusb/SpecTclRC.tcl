
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

if {$tcl_platform(os) != "Windows NT"} {
	puts -nonewline "Loading TKCon console..."
	source $SpecTclHome/Script/tkcon.tcl
	puts "Done."
}


puts "Adding SpecTcl exec directory to auto_path"

set llnlSpecTcl [file dirname [info script]]
lappend auto_path $llnlSpecTcl

puts "done"

tk appname SpecTcl-[exec hostname]-[pid]

puts "Sourcing histogram config file"


puts "Configuration file: $configFile"

source [file join $llnlSpecTcl spectclSetup.tcl]
puts "done"

puts -nonewline "Starting treeparamgui..."
source $SpecTclHome/Script/SpecTclGui.tcl
puts " Done"

