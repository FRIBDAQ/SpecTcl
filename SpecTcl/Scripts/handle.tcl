# *************************************************************************
# handle.tcl
# by Chase Evan Bolen
# This file handles load commands from files saved from SpecTcl by 
# saveall command.  It catches errors in those commands, and corrects
# them if they are anticpated errors.
# *************************************************************************

proc handle {command} {
    set qmessage "(S)kip, continue query / (O)verwrite, continue query /\n overwrite (A)ll / overwrite (N)one (skip all)?"
    upvar soverwrite soverwrite
    upvar poverwrite poverwrite
    if [catch $command result] {
	set what [lindex $command 0]
	switch $what {
	    parameter {set action $poverwrite}
	    spectrum {set action $soverwrite}
	    apply -
	    gate {
		puts stderr "Can't complete command: $command /n $result"
		return -1
	    }
	   
	}
	set def [definer $result]
	set problem [lindex $def 0]
	if {[expr {$problem != "unknown"}] && [expr {$problem != "invalid"}]} {
	    if {$action == "query"} {
		puts stderr "Error creating $what"
		puts stderr "[lrange $def 0 1]: [lindex $def 2]"
		set respond [query $qmessage]
		if {$respond == "a"} {
		   switch $what {
		       parameter {set poverwrite overwrite}
		       spectrum {set soverwrite overwrite}
		   }
		} elseif {$respond == "n"} {
		   switch $what {
		       parameter {set poverwrite skip}
		       spectrum {set soverwrite skip}
		   }
		}
		switch $respond {
		    o -
		    a {set action overwrite}
		    s -
		    n {set action skip}
		}
	    }
	    switch $action {
		overwrite {actoverwrite $command $def}
		skip {
		    puts stdout "Skipping command: \n $command"
		    return "Skipping command: \n $command"
		}
	    }
	} else {
	    if {[expr {$problem == "unknown"}] || [expr {$problem == "invalid"}]} {
		puts stderr "Unknown error creating $what"
		puts stderr $result
	    }
	}
    }
    
}
proc query {question} {
    set yn -1
    while {$yn == "-1"} {
	puts stdout $question
	gets stdin yorn 
	switch -glob $yorn {
	    S* -
	    s* { set yn s }
	    N* -
	    n* { set yn n }
	    a* -
	    A* { set yn a }
	    o* -
	    O* { set yn o }
	}
    }
    return $yn
}
# procedure takes error messages and distills to pertinant information
proc definer {msg} {
    set problem [lindex $msg 0]
    if {$problem == "Duplicate"} {
	set key [lindex $msg 7]
	set id [lindex $msg 10]
	if {$key == "-undefined-"} {
	    if {$id != "-undefined-"} {
		set problem2 id
		set name $id
	    }
	}
	if {$id == "-undefined-"} {
	    if {$key != "-undefined-"} {
		set problem2 key
		set name $key
	    }
	}
	return "$problem $problem2 $name"
    }
    if {$problem == "invalid"} {
	return invalid
    } else {
	return unknown
    }
}
proc actoverwrite {command problem} {
    switch [lindex $command 0] {
	parameter {
	    set name [lindex $problem 2]
	    switch [lindex $problem 1] {
		key {
		    set fullname [parameter -list $name]
		    if [catch "parameter -delete $name" res] {
			puts stderr "Unable to delete parameter: $name \n $res"
		    } else {
			puts stdout "[lindex $command 1]> Parameter deleted: $fullname"
			if [catch $command result] {
			    set prob2 [definer $result]
			    if {[expr {[lindex $prob2 0] == "Duplicate"}] && [expr {[lindex $prob2 1] == "id"}]} {
				set fullname [parameter -list -id [lindex $prob2 2]]
				if [catch "parameter -delete -id [lindex $prob2 2]" res3] {
				    puts stderr "Unexpected error: \n $res3"
				} else {
				    puts stdout "[lindex $command 1]> Parameter deleted: $fullname"
				    if [catch $command $result] {
					puts stderr "Unexpected error: \n $res3"
				    } else {
					puts stdout "[lindex $command 1]> Parameter overwritten: $command"
				    }
				}
			    }  else {
				puts stderr "Unexpected error: \n $result"
			    }			    
			} else {
			    puts stdout "[lindex $command 1]> Parameter overwritten: $command"
			}
		    }
		}
		id {
		    set fullname [parameter -list -id $name]
		    if [catch "parameter -delete -id $name" res] {
			puts stderr "Erase failed : \n parameter -delete -id $name \n $res"
		    } else {
			puts stdout "[lindex $command 1]> Parameter deleted: $fullname"
		    }
		    if [catch $command result] {
			puts stderr "Command failed: \n $command \n $result"
		    } else {
			puts stdout "[lindex $command 1]> Parameter overwritten: $command"
		    }
		}	
	    }
	}
	spectrum {
	    set name [lindex $problem 2]
	    if [catch "spectrum -delete $name" res] {
		puts stderr "Erase failed : \n spectrum -delete $name \n $res"
	    } else {
		puts stdout "$name> Spectrum deleted: $name"
	    }
	    if [catch $command result] {
		puts stderr "Command failed: \n $command \n $result"
	    } else {
		puts stdout "$name> Spectrum overwritten: \n $command"
	    }
	}
    }   
}