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

puts -nonewline "Loading TKCon console..."
source $SpecTclHome/Script/tkcon.tcl
puts "Done."

