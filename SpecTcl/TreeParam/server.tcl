# This implementation of TreeParameter is based on the ideas and original code of::
#    Daniel Bazin
#    National Superconducting Cyclotron Lab
#    Michigan State University
#    East Lansing, MI 48824-1321
#

proc ServerAccept {cid addr port} {
	global ClientState
	puts "Client established connection on $cid, address $addr, port $port"
	fileevent $cid readable "ServerHandle $cid"
	fconfigure $cid -buffering line -blocking 0 -buffersize 65536
	set ClientState Up
}

proc ServerHandle {cid} {
	global ClientState
	if {[gets $cid request] < 0} {
		close $cid
		puts "Client $cid has closed the connection"
		set ClientState Down
	} else {
		append buffer $request "\n"
		if {[info complete $buffer]} {
			set request $buffer
			set buffer ""
			if {[catch {eval $request} result] == 0} {
				puts $cid $result
			} else {
				puts $cid "Server has detected an error: $result"
			}
			puts $cid Done
		}
	}
}
