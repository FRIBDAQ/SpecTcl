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
    
    $saveset applyGate slice s2
    
    foreach app [$saveset listApplications] {
        set gate [dict get $app gate]
        set spec [dict get $app spectrum]
        puts "$gate is applied to $spec"
    }
    
    $saveset destroy
    $db destroy
} msg]



if {$status} {
    puts stderr "error: $msg"
}