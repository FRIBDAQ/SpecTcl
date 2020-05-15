#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}


lappend auto_path [file join $::env(SPECTCLHOME) TclLibs]
package require SpecTclDB

if {[llength $argv] != 1} {
    puts stderr "Usage: pardef.tcl database-file"
    exit -1
}

set status [catch {
    set db [DBTcl connect [lindex $argv 0]]
    set saveset [$db getSaveset "a saveset"]
    
    $saveset createParameter p1 100
    $saveset createParameter p2 101 -10.0 10.0 100 "cm"
    
    puts "Parameters defined:"
    foreach param [$saveset listParameters] {
        puts [dict get $param name]
    }
    
    $saveset destroy
    $db destroy
} msg]


if {$status} {
    puts stderr "error: $msg"
}