#!/usr/bin/tclsh

##
#  Take a treeparam dump and turn it into the C++ code to 
#  replicate the tree parameters.
#
# 

#  Read the file (first parameter) -- we'll make params.cpp
#  as output unconditionally.
#


set fd [open [lindex $argv 0] r]
set paramdefs [read $fd]
close $fd


set fd [open params.cpp w]
puts $fd "#include <config.h>"
puts $fd "#include <TreeParameter.h>"

set idx 0;
foreach param $paramdefs {
    incr idx
    set name [lindex $param 0]
    set nch  [lindex $param 1]
    set low  [lindex $param 2]
    set hi   [lindex $param 3]
    set units [lindex $param 5]

    puts $fd "CTreeParameter p$idx\(\"$name\", $nch, \(float\)$low, \(float\)$hi, \"$units\"\);"
    
}
close $fd
