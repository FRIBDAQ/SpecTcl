#
#  CopyGates.tcl
#   Implements the
#     CopyGate srcname dstname {newparams}
#   command which makes a new gate on the newparams which duplicates the
#   gate points in srcname.
#   Restrictions:
#     srcname - must exist as a gate.
#     dstgate - Must not exist as a gate.
#     newparams - Must exist and have the same resolution as old parameters.
#

#
#  Return the definition of a gate or "" if there is no such gate.
#
proc GateDefinition {name} {
    foreach gate [gate -list] {
	if {[lindex $gate 0] == $name} {
	    return $gate
	}
    }
    return ""
}
#
#  Returns a list which is the list of points in a primitive gate.
#  tosses an error if the gate was not primitive.
#   $gate is a gate definition string from [gate -list gname]
#
proc GetPointList {gate} {
    set gatetype [lindex $gate 2]
    set defstring [lindex $gate 3]
    switch -exact $gatetype {
	s {			;#  Slice gate
	     return [lindex $defstring 1]   
          }
        b -   
	c {                     ;# Contour gate (same struct as band).
	     return [lrange $defstring 1 end] 
          }
        default {               ;# Any other type is an error.
	          return -code 1 "Gate is not primitive"
                }
    }
}
#
#  Returns the list of parameters which are used by a gate
#
proc GetParameterList {gate} {
    set gatetype [lindex $gate 2]
    set defstring [lindex $gate 3]
    switch -exact $gatetype {
	s -
	b -
	c {
	    return [lindex $defstring 0]
   	  }
	gs {
	    return [lindex $defstring 0]
	}
	gb {
	    return [lindex $defstring 0]
	}
	gc {
	    return [lindex $defstring 0]
	}
	default {
	    return -code 1 "Gate is not primitive"
	}
    }
}
#
#  Return the definition of a parameter or "" if there is no such parameter.
#
proc ParamDefinition {name} {
    if {[catch {parameter -list $name} def] != 0} {
	return ""
    }
    return $def
}
#
#  Return the resolution of a parameter:
#    def - parameter definition string.
proc ParamResolution {def} {
    return [lindex $def 2]
}
#
#  GenSlice  - Generate a slice gate:
#    name  - Name of the gate.
#    param - Name of parameter on gate (assumed valid)
#    point - lower/upper limit point.
#
proc GenSlice {name param point} {
    set gatecmd "gate $name s "
    lappend defstr $param            ;# It might have spaces and need quoting.
    lappend defstr $point            ;# It will have spaces and need quoting.
    lappend gatecmd $defstr          ;# Quote the entire defstring.
    return [eval $gatecmd]
}
#
#  Gen2dGate - Generate a 2d gate (contour or band):
#   name      - Name of the gate.
#   corb      - c - contour b - band others are errors.
#   paramlist - List of parameters which make up the gate.
#   pointlist - List of gate points.
#
proc Gen2dGate {name corb paramlist pointlist} {
    if {($corb != "b") && ($corb != "c")} {
	return -code 1 "Gate must be contour or band"
    }
    set gatecmd "gate $name $corb"
    foreach param $paramlist {   ;# Put parameters separately in deflist.
	lappend deflist $param
    }
    lappend deflist $pointlist   ;# Put points in as a single entry.
    lappend gatecmd $deflist
    return [eval $gatecmd]       ;# define the gate.
}
#
#  CopyGate - Copies a gate to a new set of parameters.
#    name1   - Name of original gate.
#    name2   - Name of new gate.
#    params  - Name of new parameters for new gate.
#
#   Descriptive error on failure; nothing on success (just like the gate
#   command.
#
proc CopyGate {name1 name2 params} {

    set OldGate [GateDefinition $name1]
    if {[string length $OldGate] == 0} {
	return -code 1 "Source gate: $name1 does not exist"
    }

    if {[string length [GateDefinition $name2]] != 0} {
	return -code 1 "Destination gate: $name2 already exists"
    }

#
#      Now we have bunches of checks on the parameters:
#      Must be as many dst parameters as src.
#      All dst parameters must exist.
#      All dst parameters must have same resolution as src.
#
    set srcParams [GetParameterList $OldGate]  ;# Errors if not primitive gate
    if {[llength $srcParams] != [llength $params]} {
	set result "There are [llength $srcParams] source parameters but "
	append result "[llength $params] destinations."
	return -code 1 $result
    }
    foreach param $params { 
	set pdef($param) [ParamDefinition $param]
	
	if { [string length $pdef($param)] == 0 }  {
	    return -code 1 "Destination parameter $param does not exist"
	}
    }
    for {set i 0} {$i < [llength $srcParams]} {incr i} {
	set srcRes [ParamResolution [parameter -list [lindex $srcParams $i]]]
	set dstRes [ParamResolution $pdef([lindex $params $i])]
	if {$srcRes != $dstRes} {
	    set result "src Parameter [lindex $srcParams $i] has $srcRes bits "
	    append result "but dst Parameter [lindex $params $i] has $dstRes "
	    append result "bits"
	    return -code 1 $result
	}
    }
#
#     Now we know it's safe to copy the gate... just need to get its type.
#
    set gatetype [lindex $OldGate 2]
    puts $OldGate
    puts $gatetype
    switch -exact $gatetype {
	s {
	    GenSlice $name2 $params [GetPointList $OldGate]
	}
	b -
	c {
	    Gen2dGate $name2 $gatetype  $params [GetPointList $OldGate]
	}
	default {
	    return -code 1 "Invalid gate type"
	}
    }
    
}
