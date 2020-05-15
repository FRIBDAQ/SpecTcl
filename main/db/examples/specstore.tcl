#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}


lappend auto_path [file join $::env(SPECTCLHOME) TclLibs]
package require SpecTclDB

if {[llength $argv] != 1} {
    puts stderr "Usage: specstore.tcl database-file"
    exit -1
}

set status [catch {
    set db [DBTcl connect [lindex $argv 0]]
    set saveset [$db getSaveset "a saveset"]

    $saveset storeChannels s1 \
        [list {100 123} {101 2000} {102 400} {103 60}]
    
    set channels [$saveset getChannels s1]
    puts "Channels in s1: "
    foreach chan $channels {
        set x [lindex $chan 0]
        set v [lindex $chan 2]
        puts "Channel $x:   $v"
    }
    $saveset destroy
    $db destroy
} msg]



if {$status} {
    puts stderr "error: $msg"
}