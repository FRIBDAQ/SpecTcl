proc StartSpecTclGUI {port} {
	global SpecTclHome guishell
        global ClientState
        puts "Starting gui connecting to port $port"
	set guishell [open "|wish" "w+"]
        puts "puts $guishell set serverport $port"
        puts $guishell "set serverport $port" 
        flush $guishell
	puts $guishell "set SpecTclHome $SpecTclHome"
	flush $guishell
	puts $guishell "source $SpecTclHome/contrib/treeparam/TopGui.tcl"
	flush $guishell
        set ClientState "Up"
}

proc StopSpecTclGUI {} {
	global guishell
        global ClientState
	puts $guishell exit
	flush $guishell
        set ClientState Down
    
}

proc StartStopSpecTclGUI {} {
	global ClientState
        global serverport
    if {[string compare $ClientState Up] == 0} {
        StopSpecTclGUI
    } else {
        StartSpecTclGUI $serverport
    }
}

proc UpdateStartStopGUI {name element op} {
	global ClientState
	if {[string compare $ClientState Up] == 0} {
        .startstopgui configure -text "Stop SpecTcl GUI"
    } else {
        .startstopgui configure -text "Start SpecTcl GUI"
	}
}    

proc Exit {} {
    if {[string compare [.startstopgui cget -text] "Stop SpecTcl GUI"] == 0} {
        StopSpecTclGUI
    }
    exit
}

proc LoadSpectrumDefinition {file} {
	set handle [open $file r]
	set definitionFile [lindex [split $file /] end]
	set extension [lindex [split $definitionFile .] end]

	set currentSpectra [spectrum -list]
	foreach e $currentSpectra {
		append spectrumList [lindex $e 1] " "
	}

# Old style definition file is just a list of lists
	if {[string compare $extension sdef] == 0} {
		set theList [read $handle]
		close $handle
		set theSpectrumList [lindex $theList 0]
		set theTreeParameterList [lindex $theList 1]
		set theGateList [lindex $theList 2]
		set theApplyList [lindex $theList 3]
		foreach spectrum $theSpectrumList {
			set id [lindex $spectrum 0]
			set name [lindex $spectrum 1]
			set type [lindex $spectrum 2]
			set parameters [lindex $spectrum 3]
			set resolutions [lindex $spectrum 4]
			set data [lindex $spectrum 5]
# We need to check if spectra definitions attempt to redefine existing spectra
# and delete them if it is the case (no implicit overwriting)
			if {[info exist spectrumList] == 1 && [lsearch $spectrumList $name] != -1} {
				spectrum -delete $name
			}
			spectrum $name $type $parameters $resolutions $data
		}
		foreach treeparameter $theTreeParameterList {
			set name [lindex $treeparameter 0]
			set start [lindex $treeparameter 2]
			set stop [lindex $treeparameter 3]
			set increment [lindex $treeparameter 4]
			set unit [lindex $treeparameter 5]
			treeparameter -set $name $start $stop $increment $unit
		}
		foreach gate $theGateList {
			set name [lindex $gate 0]
			set type [lindex $gate 2]
			set description [lindex $gate 3]
			set parameters [lindex $description 0]
			set data [lreplace $description 0 0]
			set gatecmd "gate $name"
			switch -- $type {
				c -
				b {
					set des "[lindex $parameters 0] [lindex $parameters 1] \{$data\}"
					lappend gatecmd $type $des
				}
				s {
					set des "[lindex $parameters 0] $data"
					lappend gatecmd $type $des
				}
				default {
					lappend gatecmd $type $description
				}
			}
			eval $gatecmd
		}
		foreach apply $theApplyList {
			set spectrum [lindex $apply 0]
			set gate [lindex $apply 1]
			set gatename [lindex $gate 0]
			set gatetype [lindex $gate 2]
			if {[string compare $gatetype T] != 0} {
				apply $gatename $spectrum
			}
		}
	}

# New style definition file is a Tcl script
	if {[string compare $extension tcl] == 0} {
		while {![eof $handle]} {
			gets $handle line
# We need to check if spectra definitions attempt to redefine existing spectra
# and delete them if it is the case (no implicit overwriting)
			if {[string compare spectrum [lindex $line 0]] == 0} {
				set name [lindex $line 1]
				if {[info exist spectrumList] == 1 && [lsearch $spectrumList $name] != -1} {
					spectrum -delete $name
				}
			}
# That's the nice thing about saving a script!
			eval $line
		}
		close $handle
	}
	sbind -all
}

proc SaveSpectrumDefinition {file} {
	set handle [open $file w]
	set definitionFile [lindex [split $file /] end]

	set spectrumList [spectrum -list]

# here we check if the version of the treeparameter command has the option 
# check implemented
	if {[string match *check* [treeparameter]]} {
		set treeList [treeparameter -list]
		set changedTreeList ""
		foreach tree $treeList {
			if {[treeparameter -check [lindex $tree 0]]} {
				append changedTreeList [treeparameter -list [lindex $tree 0]]
			}
		}
		set treeparameterList $changedTreeList
	} else {
		set treeparameterList [treeparameter -list]
	}
	
	set gateList [SortGates [gate -list]]

	set applyList [apply -list]

	set extension [lindex [split $definitionFile .] end]
	if {[string compare $extension sdef] == 0} {
		set theList ""
		lappend theList $spectrumList
		lappend theList treeparameterList
		lappend theList $gateList
		lappend theList $applyList
		puts $handle $theList
	}
	if {[string compare $extension tcl] == 0} {
		puts $handle "# Spectrum definitions"
		foreach spectrum $spectrumList {
			set id [lindex $spectrum 0]
			set name [lindex $spectrum 1]
			set type [lindex $spectrum 2]
			set parameters [lindex $spectrum 3]
			set resolutions [lindex $spectrum 4]
			set data [lindex $spectrum 5]
			puts $handle "spectrum $name $type \{$parameters\} \{$resolutions\} $data"
		}
		puts $handle "# TreeParameter scaling"
		foreach treeparameter $treeparameterList {
			set name [lindex $treeparameter 0]
			set start [lindex $treeparameter 2]
			set stop [lindex $treeparameter 3]
			set increment [lindex $treeparameter 4]
			set unit [lindex $treeparameter 5]
			puts $handle "treeparameter -set $name $start $stop $increment $unit"
		}
		puts $handle "# Gate definitions"
		foreach gate $gateList {
			set name [lindex $gate 0]
			set type [lindex $gate 2]
			set description [lindex $gate 3]
			set parameters [lindex $description 0]
			set data [lreplace $description 0 0]
			set gatecmd "gate $name"
			switch -- $type {
				c -
				b {
					set des "[lindex $parameters 0] [lindex $parameters 1] \{$data\}"
					lappend gatecmd $type $des
				}
				s {
					set des "[lindex $parameters 0] \{$data\}"
					lappend gatecmd $type $des
				}
				default {
					lappend gatecmd $type $description
				}
			}
			puts $handle $gatecmd
		}
		puts $handle "# Apply definitions"
		foreach apply $applyList {
			set spectrum [lindex $apply 0]
			set gate [lindex $apply 1]
			set gatename [lindex $gate 0]
			set gatetype [lindex $gate 2]
			if {[string compare $gatetype T] != 0} {
				puts $handle "apply $gatename $spectrum"
			}
		}
	}
	close $handle
}

proc SortGates {theList} {
	if {[string compare $theList ""] == 0} {return ""}
	# first sort primitive and composite gates
	foreach g $theList {
		set type [lindex $g 2]
		if {[string compare $type *] == 0} {
			lappend composites $g
			lappend compgates [lindex $g 0]
		} elseif {[string compare $type +] == 0} {
			lappend composites $g
			lappend compgates [lindex $g 0]
		} elseif {[string compare $type -] == 0} {
			lappend composites $g
			lappend compgates [lindex $g 0]
		} else {
			lappend primitives $g
			lappend primgates [lindex $g 0]
		}
	}
	# then flag composite gates which depend on other composites
	set last [expr [llength $composites] - 1]
	for {set i 0} {$i <= $last} {incr i} {
		set g [lindex $composites $i]
		set gates [lindex $g 3]
		set flag 0
		set swap 0
		foreach gg $gates {
			if {[lsearch $primgates $gg] == -1} {
				set flag 1
				break
			}
		}
		# if flagged composite depends on other composite listed later push it to the end
		if {$flag} {
			foreach gg $gates {
				for {set j [expr $i+1]} {$j <= $last} {incr j} {
					if {[string compare $gg [lindex $compgates $j]] == 0} {
						set swap 1
						break
					}
				}
			}
		}
		if {$swap} {
			set composites [lswap $composites $i $last]
			set compgates [lswap $compgates $i $last]
		}
	}
	# rebuild the whole list and we are done!
	foreach g $primitives {
		lappend newlist $g
	}
	foreach g $composites {
		lappend newlist $g
	}
	return $newlist
}

proc lswap {list index1 index2} {
	set el1 [lindex $list $index1]
	set el2 [lindex $list $index2]
	set newlist [lreplace $list $index1 $index1 $el2]
	set newlist [lreplace $newlist $index2 $index2 $el1]
	return $newlist
}

source $SpecTclHome/contrib/treeparam/server.tcl

set serverport 9111
set notok         1

while {$notok} {
    if {[catch "set spectclserver [socket -server ServerAccept $serverport]"] == 0} {
	set notok 0
    } else {
	incr serverport
    }
}

trace variable ClientState w UpdateStartStopGUI
button .startstopgui -text "Start SpecTcl GUI" -command StartStopSpecTclGUI
pack .startstopgui -expand 1 -fill both -side bottom

StartSpecTclGUI $serverport


