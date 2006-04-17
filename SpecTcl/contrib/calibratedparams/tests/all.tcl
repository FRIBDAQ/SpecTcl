

lappend auto_path ..;			# Satisfy packages from up one  level.

#  Load the TCL Test packages.

if {[lsearch [namespace children] ::tcltest] == -1 } {
    package require tcltest
    namespace import ::tcltest::*
}

#  Run all tests in this directory:

set ::tcltest::testSingleFile false
set ::tcltest::testsDirectory [file dir [info script]]


#  Run the tests (.test files).

foreach file [::tcltest::getMatchingFiles] {
    if {[catch {source $file} msg]} {
	puts stdout $msg
    }
}

::tcltest::cleanupTests 1
return