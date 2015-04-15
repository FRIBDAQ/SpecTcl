lappend auto_path [file join $SpecTclHome TclLibs]
package require daqdefs

source ../config/hardware.tcl
attach -pipe [file join $::daqdefs::daqbin spectcldaq]
start
