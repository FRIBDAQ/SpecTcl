# *************************************************************************
# listall.tcl
# by Chase Evan Bolen
# script adding formatted output commands for spectra, gates, and applys
# to SpecTcl
# *************************************************************************

proc SpecList {{slist ""} {parformat down}} {
    if {$slist == ""} {
	set slist [spectrum -list]
    }
    set output "Name\t\tId\tRes\tType\tData\tParameter List\n"
    foreach spec $slist {
	set name [lindex $spec 1]
	if {[string length $name] <8} {
	    set name "$name\t\t"
	} else {
	    set name "$name\t"
	}
	set line  "$name[lindex $spec 0]\t[lindex $spec 4]\t[lindex $spec 2]\t[lindex $spec 5]\t"
	set i 1
	switch -glob $parformat {
	    l* {append line "[lindex $spec 3]\n"}
	    d* {
		foreach par [lindex $spec 3] { 
		    if {$i > 1} {
			append line "\t\t\t\t\t\t$par\n"
		    } else {
			append line "$par\n"
		    }
		    incr i
		}
	    }
	    w* {
		foreach par [lindex $spec 3] {
		    if {[llength [lindex $spec 3]] >2} {
			if ![expr {$i%2}] {
			    append line "$par\n"
			} else {
			    if {$i>1} {
				append line "\t\t\t\t\t\t$par "
			    } else {
				append line "$par "
			    }	
			    
			}
		    } else {
			append line "[lindex $spec 3]\n"
			break
		    }
		    incr i
		}
	    }
	}
	append output "$line\n"
    }
    return $output
}
proc GateList {{glist ""} {option cdown}} {
    if {$glist == ""} {
	set glist [gate -list]
    }
    set output "Name\t\tId\tType\tDescription\n"
    foreach gate $glist {
	append output [outgate $gate $option]
    }
    return $output
}
proc ApList {{alist ""} {option cdown}} {
    if {$alist == ""} {
	set alist [apply -list]
    }
    set output "Name\t\tGate\t\tId\tType\tDescription\n"
    foreach ap $alist {
	if {[lindex [lindex $ap 1] 0] != "-TRUE-"} {
	    set name [lindex $ap 0]
	    if {[string length $name] <8} {
		set name "$name\t\t"
	    } else {
		set name "$name\t"
	    }
	    set nd [outgate [lindex $ap 1] $option 2]
	    set line "$name$nd"
	} else {
	    set line ""
	}
	append output $line
    }
    return $output
}

proc outgate {gate option {tabslots 0}} {
    set name [lindex $gate 0]
    set tabs ""
    if {$tabslots > 0} {
	for {set n 1} {$n <=$tabslots} {incr n} {
	    set tabs "$tabs\t"
	}
    }
    if {[string length $name] <8} {
	set name "$name\t\t"
    } else {
	set name "$name\t"
    }
    set desc [lindex $gate 3]
    switch -glob $option {
	a* {
	    if {[string length $desc] > [expr {47 - $tabslots * 8}]} {
		if {[llength $desc] > 1} {
		    for {set i 0} {$i <[llength $desc]} {incr i} {
			if {[expr {[string length [lrange $desc 0 $i]] > [expr {47 - $tabslots * 8}]}] && [expr {$i>0}]} {
			    set desc "[lrange $desc 0 [expr {$i - 1}]]\n\t\t\t\t$tabs[lrange $desc $i end]"
			    break
			}
		    }
		}
		
	    }
	}
	d* {
	    set nd [lindex $desc 0]
	    foreach ele [lrange $desc 1 end] {
		set nd "$nd\n\t\t\t\t$tabs$ele"
	    }
	    set desc $nd
	}
	c* {
	    if {[expr {[lindex $gate 2] == "c"}] || [expr {[lindex $gate 2] == "b"}] || [expr {[lindex $gate 2] == "gc"}] || [expr {[lindex $gate 2] == "gb"}] || [expr {[lindex $gate 2] == "gs"}]} {
		set nd [lindex $desc 0]
		foreach ele [lrange $desc 1 end] {
		    set nd "$nd\n\t\t\t\t$tabs$ele"
		}
		set desc $nd
	    }
	}
    }
    set line  "$name[lindex $gate 1]\t[lindex $gate 2]\t$desc"

    return "$line\n"
}
#   Procedure to format a set of parameter definition lists
#   into a nice output string which can be used as desired.
#   Typical use might be:
#       
#     ParList { [parameter -list -byid] }
#
proc ParList { {ParameterList ""} } {
    if {$ParameterList == ""} { set ParameterList [parameter -list] }
    set output "Name\t\tId\t\tResolution\n"
    set fmt    "%s \t %d  \t\t %d \n"
    foreach Parameter $ParameterList {
	set line [format $fmt [lindex $Parameter 0] [lindex $Parameter 1] [lindex $Parameter 2]]
	append output $line

    }
    return $output
}
proc More { output } {
  set output "$output\n"         ;# Ensure trailing newline.
  set termsize 23
  set line     0
  set index    0
  while { [set newline [string first "\n" [string range $output $index end]]] != -1 } {
      set newline [expr $newline + $index ]
      set outline [string range $output $index $newline]
      set index $newline
      incr index
      puts -nonewline $outline
      incr line
      if { $line >= 23 } {
	  puts "--more--"
	  set line 0
	  set junk [gets file0]
	  exec clear
      }
  }
}


