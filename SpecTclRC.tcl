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

set here $llnlSpecTcl

puts "done"

tk appname SpecTcl-[exec hostname]-[pid]

puts "Sourcing histogram config file"


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

    spectrum 2d.$enum 2 [list $ename $tname] {{0 4095 512} {0 4095 512}}
}


sbind -all

.gui.b update

#

#
puts  -nonewline "Starting scaler display.."
set scalerWin [toplevel .scalers]
source /usr/opt/daq/current/TclLibs/ScalerDisplay/scaler.tcl
if {[file exists [file join ~/config scalerdisplay.tcl]]} {
    source [file join ~/config scalerdisplay.tcl]
}
puts " done"

#
# Adding the rates GUi to the scaler display and folder GUI:
#

puts -nonewline "Adding rates GUI "
puts "Loading scripts from $here"
source [file join $here ratesGui.tcl]
puts Done.

#
#  Loading the rates.tcl file from ~/config if it exists:
#

set ratesFile [file join ~ config rates.tcl]
if {[file exists $ratesFile]} {
    puts -nonewline "Found rates file $ratesFile restoring it "
    source $ratesFile
    puts Done.
}
#
#  Load the projection gui
#
puts -nonewline "Loading projection GUI"
source [file join $here project.tcl]
puts " Done"

#
#  Load the figure of merit tcl code:
#
puts -nonewline "Loading FOM Tcl Scripts "
source [file join $here fom.tcl]
puts  Done