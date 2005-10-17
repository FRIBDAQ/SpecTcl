parameter george 50
for {set i 60} {$i < 70} {incr i} {
    parameter not.tree.parameter.$i $i
}

parameter time 100
set clocktime 0


sbind -all

lappend auto_path [file dirname [info script]]
package require SpecTclGui

