#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}


lappend auto_path [file join $::env(SPECTCLHOME) TclLibs]
package require SpecTclDB

if {[llength $argv] != 1} {
    puts stderr "Usage: vardef.tcl database-file"
    exit -1
}

set status [catch {
    set db [DBTcl connect [lindex $argv 0]]
    set saveset [$db getSaveset "a saveset"]

    $saveset createVariable slope 1.23 KeV/lsb
    $saveset createVariable offset 10  KeV
    $saveset createVariable gainmatch 1.23
    
    puts "Tree variables: "
    foreach var [$saveset listVariables] {
        set name [dict get $var name]
        set val  [dict get $var value]
        set units [dict get $var units]
        
        puts "$name : $val$units"
    }
    
    $saveset destroy
    $db destroy
} msg]



if {$status} {
    puts stderr "error: $msg"
}