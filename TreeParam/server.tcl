# (C) Copyright Michigan State University 2019, All rights reserved 
# Copyright (c) 1999, Bryan Oakley
# All Rights Reservered

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
