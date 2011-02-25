proc doubleg event {
    puts $event
    set trigger [lindex $event 0]
    array set parameters [lindex $event 1]
    if {[array names parameters george] eq "george"} {
	puts "match"
  	set value [expr $parameters(george) * 2.0]
	return [list $trigger [list double $value]]
    }
    return [list $trigger [list]]
}
