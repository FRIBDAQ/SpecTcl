#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}


lappend auto_path [file join $::env(SPECTCLHOME) TclLibs]
package require SpecTclDB

if {[llength $argv] != 1} {
    puts stderr "Usage: specdef.tcl database-file"
    exit -1
}

set status [catch {
    set db [DBTcl connect [lindex $argv 0]]
    set saveset [$db getSaveset "a saveset"]
    
    $saveset createSpectrum s1 1 p1 [list [list 0 1023 1024]]
    $saveset createSpectrum s2 2  \
        [list p1 p2] [list [list 0 1023 1024] [list -10.0 10.0 100]]
    
    puts "Spectra in saveset:"
    foreac spectrum [$saveset listSpectra] {
        puts [dict get $spectrum name]
    }
} msg]

$saveset destroy
$db destroy

if {$status} {
    puts stderr "error: $msg"
}