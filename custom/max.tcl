##
#  tcl::mathfunc::max has a problem with json wanting more
#  than 2 args.. if nobody else has patched that we will.
#

if {[catch {max(1,2,3)}]} {
    proc tcl::mathfunc::max {args} {
	if {[llength $args] == 1} {
	    return $args
	}
	set biggest [lindex $args 0]
	foreach item $args {
	    if {$item > $biggest} {set biggest $item}
	}
	return $biggest

    }
}
