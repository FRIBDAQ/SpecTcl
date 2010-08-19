#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2009.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321


# (C) Copyright Michigan State University 2014, All rights reserved 
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

if {$tcl_platform(os) != "Windows NT"} {
	puts -nonewline "Loading TKCon console..."
	source $SpecTclHome/Script/tkcon.tcl
	puts "Done."
}

puts -nonewline "Starting treeparamgui..."
source $SpecTclHome/Script/newGui.tcl
puts " Done"

puts "Adding SpecTcl exec directory to auto_path"

set llnlSpecTcl [file dirname $argv0]
lappend auto_path $llnlSpecTcl

puts "done"

tk appname SpecTcl-[exec hostname]-[pid]

puts "Sourcing histogram config file"

set here [file dirname [info script]]
source [file join $here  spectclSetup.tcl]
puts "done"


#
#  Create the 2-d spectra for the e vs. t
#
set parameters [parameter -list e.*]
foreach edef $parameters {
    set ename [lindex $edef 0]
    set enum  [lindex [split $ename .] 1]
    set tname t.$enum

    puts "spectrum 2d.$enum 2 [list $ename $tname] {{0 8192 512} {0 8192 512}}"
    spectrum 2d.$enum 2 [list $ename $tname] {{0 8192 512} {0 8192 512}}}

}

sbind -all

.gui.b update