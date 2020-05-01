#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}


lappend auto_path [file join $::env(SPECTCLHOME) TclLibs]
package require SpecTclDB

if {[llength $argv] != 1} {
    puts stderr "Usage: makesaveset.tcl database-file"
    exit -1
}

set status [catch {
    set db [DBTcl connect [lindex $argv 0]]
    set saveset [$db createSaveset "a saveset"]
    $saveset destroy
    $db      destroy
    
} msg]

if {$status} {
    puts "Error: $msg"
    exit -1
}
