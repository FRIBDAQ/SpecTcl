# *************************************************************************
# fileall.tcl
# by Chase Evan Bolen
# This script is intended to add file save/load functions to SpecTcl.
# These save: procedures, spectra, gates, and applys
# It also adds a clean function which deletes all parameters and spectra.
# *************************************************************************
global SpecTclHome
source $SpecTclHome/Script/dtree.tcl
proc getparms {{destination "stdout"}} {
    set parms [parameter -list]
    set lparms [split $parms \n]
    for {set i 0} {$i < [llength $lparms]} {incr i} {
	set ele [string trim [lindex $lparms $i] \{]
	set ele [string trim $ele \}]
	if {$ele > ""} {
	    puts $destination "handle \"parameter $ele\""
	}    
    }
}

proc getspects {{destination "stdout"}} {
    foreach line [spectrum -list] {
	set parms [lrange $line 1 4]
	if {[string first "\{--Deleted Parameter--\}" $parms] == -1} {
	    puts $destination "handle \"spectrum $parms\""
	} else {
	    missingparam spectrum $parms
	}
    }
}

proc saveall {filename} {
    global SpecTclHome
    set headerpath "$SpecTclHome/Script/save.head"
    set path $filename
    if [catch {open $headerpath r 0600} header] {
	puts stderr "Can't find header file: \n $header"
    } else {
	
	if [catch {open $path w 0600} file] {
	    puts stderr "Can't open $path: $file"
	} else {
	    puts $file [read $header]
	    # this procedure only writes the header to the file.  it
	    # passes the filename to the get_____ procedures which
	    # do the formatting and output.
	    getparms $file
	    getspects $file
	    getgates $file
	    close $file
	}
    close $header
    }
}

proc loadall {filename} {
# yep, this is it.  everything else is done in the handle.tcl script.
    if [catch {source $filename} stuff] {
	puts stderr "Error loading file: $filename"
	puts stderr $stuff
    }
}

proc clean {} {
    set parlist [parameter -list]
    foreach line $parlist {
	if [catch "parameter -delete [lindex $line 0]" res2] {
	    puts stderr "Error deleting SpecTcl parameters: \n $res2"
	}
    }
    set speclist [spectrum -list]
    foreach line $speclist {
	if [catch "spectrum -delete [lindex $line 1]" res2] {
	    puts stderr "Error deleting SpecTcl spectrum: \n $res2"
	}
    }
    set gatelist [gate -list]
    foreach line $gatelist {
	if [catch "gate -delete [lindex $line 0]" res2] {
	    puts stderr "Error deleting SpecTcl gate: \n $res2"
	}
    }
}

proc getgates {{destination "stdout"}} {
    set glist [gate -list]
    foreach element $glist {
	set gatestring [gateformat [lreplace $element 1 1]]
	if {[string first {-Deleted Parameter-} [lindex $gatestring 2]] == -1} {
	    set Gatename [lindex $element 0]
	    additem gatetree $Gatename
	    set direct($Gatename) $gatestring
	} else {
	    missingparam gate $gatestring
	}
    }
    
    setmissingevent gatetree {
	# this sets the 'missing event' of the dependancy tree that is called when an
	# item is dependent on a missing item.  Whatever is in these brackets
	# will be executed for each item deleted because it its dependandcies are missing.
	# In this case I am calling the missingparam procedure.  I kno this is a lot of work
	# and I couldve called missingparam directly, but i wanted total separation of this script
	# and the dependancy tree structure.
	missingparam gate "[getval ptree] missing depended gate"
    }
    for {set i 0} {$i < 2} {incr i} {
	foreach element $glist {
	    set line [lreplace $element 1 1]
	    set name [lindex $line 0]
	    if {[expr {[lindex $line 1] == "+"}] || [expr {[lindex $line 1] == "*"}] || [expr {[lindex $line 1] == "-"}]} {
		dependon gatetree $name [lindex $line 2]
	    }
	}
    }
    set herelist [outlist gatetree]
    if {$herelist!="-1"} {
	foreach name $herelist {
	    puts $destination "handle \"gate $direct($name)\""
	}
	# I call getapply from here because the list of functional gates is handy ($herelist)
	getapply $destination $herelist
    }
}

proc gateformat {gatein} {
    set a [lindex $gatein 1]
    if {[expr {$a != "*"}] && [expr {$a != "+"}] && [expr {$a != "-"}]} {
	set inf [lindex $gatein 2]
	if {($a == "gs") || ($a == "gb") || ($a == "gc")}  {
	    set ps $gatein
	} elseif {[llength $inf] > 2} {
	    set ps "[lrange $gatein 0 1] \{[lindex $inf 0] \{ [lrange $inf 1 end]\}\}"
	} else {
	    set ps "[lrange $gatein 0 1] \{[lindex $inf 0] [lrange $inf 1 end]\}"
	}
    } else {
	set ps $gatein
    }
    return $ps
}

proc sepgates {gatelist} {
    if {[llength $gatelist] > 1} {
	foreach gate $gatelist {
	    foreach single [sepgates $gate] {lappend glist $single}
	}
    } else {
	return $gatelist
    }
    return $glist
}	    

proc getapply {{destination stdout} gatelist} {
    set aplist [apply -list]
    set doapply 1
    foreach ap $aplist {
	if {[lindex [lindex $ap 1] 0] != "-TRUE-"} {
	    set flag "\{--Deleted Parameter--\}"
	    set spec [lindex $ap 0]
	    # this gets rid of applys related to spectrum with deleted parameters
	    if {[string first $flag [lrange [spectrum -list $spec] 1 4]] == -1} {
		set op [lindex [lindex $ap 1] 2]
		if {[expr {$op == "+"}] || [expr {$op == "*"}] || [expr {$op == "-"}]} {
		    set gates [lreplace [lindex $ap 1] 1 2]
		    set glist [sepgates $gates]
		    foreach gate $glist {
			# this gets rid of applys with gates with missing parameters, or gates 
			# depending on gates with missing parameters, or gates depending on
			# gates depending on... well, you see what I mean.
			if {[lsearch -exact $gatelist $gate] == "-1"} {
			    set doapply 0
			}
		    }
		} else {
		    set gate [lindex [lindex $ap 1] 0]
		    if {[lsearch -exact $gatelist $gate] == "-1"} {
			set doapply 0
		    }
		}  
		if $doapply {
		    puts $destination "handle \"apply $gate $spec\""
		} else {
		    missingparam apply "either gate: $gate or spectrum: $spec is disabled"
		}
	    }
	}
    }   
}

proc missingparam {fromwhat reason} {
# this procedure is called when a spectrum, gate, or apply is not saved
# because a required parameter is missing somewhere along the line.
# fromwhat is the structure calling the process, reason can be a declaration or explanation of problem
}
