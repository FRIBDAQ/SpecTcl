#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}


lappend auto_path [file join $::env(SPECTCLHOME) TclLibs]
package require SpecTclDB

if {[llength $argv] != 1} {
    puts stderr "Usage: gatedef.tcl database-file"
    exit -1
}

set status [catch {
    set db [DBTcl connect [lindex $argv 0]]
    set saveset [$db getSaveset "a saveset"]
    
    $saveset create1dGate slice  s p1 100 200
    $saveset create2dGate contour c [list p1 p2] \
        [list {100 10} {200 100} {200 200} {100 200} ]
    $saveset createCompoundGate and * [list slice contour]
    $saveset createMaskGate  mask em p1 0x55555555
    
    puts "Gates in saveset:"
    foreach gate [$saveset listGates] {
        puts [dict get $gate name]
    }
    
    $saveset destroy
    $db destroy
} msg]



if {$status} {
    puts stderr "error: $msg"
}