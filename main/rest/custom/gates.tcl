##
## Application direct URL for SpecTcl gate definitions
#
package require json::write
namespace eval ::SpecTcl {
    array set minGateXYPoints [list b 2 c 3 gb 2 gc 3];	# min X/Y points for each 2d gate type.
}

Direct_Url /spectcl/gate SpecTcl_Gate

##
# List the gates in JSON notation
# The gates are returned as the detail attribute of the
# returned object which will be an array objects.
# Each object will have the following mandatory attributes:
# - name  - Name of the gate.
# - type  - Type of the gate
#
# The following attributes will be present or not depending on the gate type:
# - gates - For composite gates (type *, +, -,) this is an array of gate names
#           that are evaluated for this gate.
# - parameters - For gates that depend on parameters (type s,c,b,c2band,gs,gb,gc,em,am,nm)
# - points     - For gates that have x/y point lists. (c,b,c2band,gb,gc) this attribute is an array
#                of x,y points.
# - low,high   - Limits for slice like gates (s,gs).
# - value      - Mask value for em am, nm
#
# @parameter - pattern defaults to * glob patttern that describes the gates desired.
#
# @return - the JSON result object which has status and detail members.
#
proc SpecTcl_Gate/list {{pattern *}} {
    set ::SpecTcl_Gate/list application/json

    set gates [gate -list $pattern]
    set gateObjects [list];	# list of json encoded objects.


    foreach gate $gates {
	#
	# Pull out the invariant parts of the gate:

	set name [lindex $gate 0]
	set type [lindex $gate 2]
	set description [lindex $gate 3]; # The form of this varies depending on type.

	set object [list  \
				 name [json::write string $name] \
				 type [json::write string $type]]

	# The procs called from the if-chain below return a list of name type like
	# json::write object expects for the remainder of the gate:
	
	# compound gates:

	set fieldList [list]

	# Compound gates.

	if {$type in [list + - *]} {
	    lappend fieldList  gates [::SpecTcl::_jsonStringArray $description]
	}

	# slice gate

	if {$type eq "s"} {
	    lappend  fieldList  parameters [::SpecTcl::_jsonStringArray [lindex $description 0]]  \
			       {*}[::SpecTcl::_marshallLowHigh [lindex $description 1]]
	}
	if {$type eq "gs"} {
	    lappend  fieldList parameters [::SpecTcl::_jsonStringArray [lindex $description 1]] \
			       {*}[::SpecTcl::_marshallLowHigh [lindex $description 0]]
	}


	# Gates with parameters where the parameter(s) are 0 and points follow

	if {$type in [list b c ]} {
	    lappend fieldList parameters [::SpecTcl::_jsonStringArray [lindex $description 0]]
	    lappend fieldList points  [::SpecTcl::_marshallPoints  $description 1]
	}
	# Gates with parametrs the 1'th  element of the description.

	if {$type in [list gb  gc]}  {
	    lappend fieldList parameters [::SpecTcl::_jsonStringArray [lindex $description 1]]
	    lappend fieldList points [::SpecTcl::_marshallPoints [lindex $description 0]]
	}

	#mask gates:

	if {$type in [list em am nm]} {
	    lappend fieldList parameters [::SpecTcl::_jsonStringArray [lindex $description 0]]
	    lappend fieldList value    [lindex $description 1]
	}

	# Build the final gate objects item and append it:

	set object [concat $object $fieldList]

	lappend gateObjects [json::write object {*}$object]
			     
    }



    return [::SpecTcl::_returnObject "OK" [json::write array {*}$gateObjects]]

}
##
# Delete a gate... really this sets the gate to FALSE but preserves it so we don't have to 
# walk the dependency tree.  It also allows dependent gates to be later revived by a redef of just
# the deleted gate(s)
#
# @param name - Name of the gate to delete  (required)
#
# @return usual json object status = OK on success.
# Error returns include:
#   - missing parameter - name was not found.
#   - not found         - no gate of that name was found.
#   = command failed    - gate -delete command failed.
#
proc SpecTcl_Gate/delete {{name ""}} {
    set ::SpecTcl_Gate/delete application/json

    if {$name eq ""} {
	return [::SpecTcl::_returnObject "missing parameter" [json::write string name]]
    }

    set gate [gate -list $name]
    if {[llength $gate] != 1} {
	return [::SpecTcl::_returnObject "not found" [json::write string $name]]
    }

    if {[catch {gate -delete $name} msg]} {
	return [::Spectcl::_returnObject "command failed" [json::write string $msg]]
    }
    return [::SpecTcl::_returnObject]
}


##
# Modify/create a gate.  Note SpecTcl makes no distinction between creating and
# editing a gate.  Redefinition of an existing gate does just that.  Parameters below
# are query parameters... most parameters come in the 'args' trailing formal
# param. Mandatory parameters are required by all gates.
# type specific by some gate tyeps.
#
# mandatory parameters are required for all gates.
# 
#
# @param name - Name of the gate. - mandatory
# @param type - gate type.        - mandatory
# @param gate - list of gates this gate depends on - type specific *,+,-,c2and
# @param [x|y]parameter - list of parameters this gate is on  - type specific : b,c,gb,gc,gs,s,em,am,nm
# @param [x|y]coord(i) - list of x/y coordinates this gate is on b,c,gb,bc
# @param value  - Value for e.g. mask gates.
# @param low,high - Low/high limit for slices.
#
# @return the usual JSON object some stock errors are:
# - missing parameter - A required parameter is missing.
# - bad parameter     - A parameter is in some way invalid.
# - insufficient points - Too few points for the gate type.
# - command failed    - The gate creation command failed.
#
proc SpecTcl_Gate/edit {args} {
    set ::SpecTcl_Gate/edit application/json

    set queryParams [::SpecTcl::_marshallDict  $args]

    # Name and type are mandatory for all gate types:

    set missingKey [::SpecTcl::_missingKey $queryParams [list name type]]

    if {$missingKey ne ""} {
	return [::SpecTcl::_returnObject "missing Parameter" [json::write string $missingKey]]
    }
    set name [dict get $queryParams name]
    set type [dict get $queryParams type]
    
    # Type could be + which is a wonky html escape not well handled by any
    # of our stack:
    
    if {$type eq "%2B"} { set type +}

    # Should only be one name, and type:

    if {[llength $name] > 1} {

	return [::SpecTcl::_returnObject "bad parameter" \
		    [json::write string "Only one name allowed ($name)"]]
    }
    if {[llength $type] > 1} {
	return [::SpecTcl::_returnObject "bad parameter" \
		    [json::write string "Only one type allowed ($type)"]]
    }

    # The following if/else generates the gate command to perform:
    # by type... in no particular order.

    ## True false.
    
    if {$type in {T F}} {
	set command [list gate $name $type [list]]

    ## Compound gates:

    } elseif {$type in [list * + - c2band]} {
        set missingKey [::SpecTcl::_missingKey $queryParams [list gate]]
        if {$missingKey ne ""} {
            return [::SpecTcl::_returnObject  "missing parameter" [json::write string $missingKey]]
    
        }
        set gates [dict get $queryParams gate]
        if {($type eq "-") && ([llength $gates] > 1)} {
            return [::SpecTcl::_returnObject "bad parameter" \
                [json::write string "- gate can only take one gate: [join $gates \", \"]"]]
        }
        if {($type eq "c2band") && ([llength $gates] != 2)} {
            return [::SpecTcl::_returnObject "bad parameter" \
                [json::write string "c2band gate must have two contours [join $gates \", \"]"]]
        }
        set command [list gate $name $type {*}$gates]
        
    # bands and contours.

    } elseif {$type in [list b c]} {
	#
	# Need an x/y parameter as well as x/y points and the x/y point lists must match in length.
	#
	set missingKey [::SpecTcl::_missingKey $queryParams [list xparameter yparameter]]
	if {$missingKey ne ""} {
	    return [::SpecTcl::_returnObject "missing parameter" [json::write string $missingKey]]
	}
	set xparam [dict get $queryParams xparameter]
	set yparam [dict get $queryParams yparameter]
	set coordinates [::SpecTcl::_marshallXYPoints $queryParams]
	if {[llength $coordinates] < $::SpecTcl::minGateXYPoints($type)} {
	    return [::SpecTcl::_returnObject "insufficient points" [json::write string $coordinates]]
	}
	set command [list gate $name $type [list $xparam $yparam $coordinates]]
	
    # Bit mask gates.
    } elseif {$type in [list em am nm]} {
	# We need a parameter and a value:

	set missingKey  [::SpecTcl::_missingKey $queryParams [list parameter value]]
	if {$missingKey ne ""} {
	    return [::SpecTcl::_returnObject "missing parameter" [json::write string $missingKey]]
	}
	set parameter [dict get $queryParams parameter]
	set value     [dict get $queryParams value]
	set command [list gate $name $type [list $parameter $value]]


    # Slice gate:
    } elseif {$type eq "s"} {
	set missingKey [::SpecTcl::_missingKey $queryParams [list parameter low high]]
	if {$missingKey ne ""} {
	    return [::SpecTcl::_returnObject "missing parameter" [json::write string $missingKey]]
	}
	set parameter [dict get $queryParams parameter]
	set low       [dict get $queryParams  low]
	set high     [dict get $queryParams high]
	set command [list gate $name $type [list $parameter [list $low $high]]]

    # Gamm 2d gates:
     
    } elseif {$type in [list gb gc]} {
	set missingKey [::SpecTcl::_missingKey $queryParams [list parameter]]
	if {$missingKey ne ""} {
	    return [::SpecTcl::_returnObject "missing parameter" [json::write string $missingKey]]
	}

	set parameters [dict get $queryParams parameter]
	set coordinates [::SpecTcl::_marshallXYPoints $queryParams]
	if {[llength $coordinates] < $::SpecTcl::minGateXYPoints($type)} {
	    return [::SpecTcl::_returnObject "insufficent points" [json::write string $coordinates]]
	}

	set command [list gate $name $type [list $coordinates $parameters]]
    
    # Gamma slice.
    } elseif {$type in [list gs]} {
	# Gamma slices will use multiple parameters and low/high as the parameter order is unimportant.
	#
	set missingKey [::SpecTcl::_missingKey $queryParams [list parameter low high]]
	if {$missingKey ne ""} {
	    return [::SpecTcl::_returnObject "missing parameter" [json::write string $missingKey]]
	}
	set params [dict get $queryParams parameter]
	set low    [dict get $queryParams low]
	set high   [dict get $queryParams high]

	set command [list gate $name $type [list [list $low $high] $params]]


    # Unrecognized gate type:

    } else {
        return [::SpecTcl::_returnObject "bad parameter" [json::write string "Invalid gate type: $type"]]
    }
    if {[catch {{*}$command} msg]} {
        return [::SpecTcl::_returnObject "command failed" [json::write string $msg]]
    }
 
    return [::SpecTcl::_returnObject ]
}

#--------------------------------------------------------------------------------
#
#  Local procs
#

##
# marshallLowHigh marshalls a low high pair into a list of the form:
# low lowvalue high highvalue
#
# @param list list contaning the low and high values
#
proc ::SpecTcl::_marshallLowHigh list {
    return [list low [lindex $list 0] high [lindex $list 1]]
}
##
# Marshall list of points into an array of x/y points in json notation.
# @param list  - list of x/y pairs
# @param start - starting index of the list.
# 

proc ::SpecTcl::_marshallPoints {list {start 0}} {
    set points [list]
    foreach point [lrange $list $start end] {
	lappend points [json::write object \
			    x [lindex $point 0] \
			    y [lindex $point 1]]
    }
    return [json::write array {*}$points]

}
##
# Given a dict of key value pairs and a list of keys that _must_ be in the
# dict, returns an emtpy string if all keys are present or the first missing key
# if a key is missing.
#
# @param dictionary - the dict to look through.
# @param keys       - list of keys to check for.
#
# @return string
# @retval "" all keys are in the dict.
# @retval first missing key in list.
#
proc ::SpecTcl::_missingKey {dictionary list} {
    foreach key $list {
	if {![dict exists $dictionary $key]} {
	    return $key
	}
    }
    return {}
}
##
# Creates a dict from a list of key/value pairs where repeated keys result in
# a list of items e.g.
#  a b a c results in a dict with one key a whose value is [list b c]
#
# @param list - the list to process
#
# @return dict as described above.
#
proc ::SpecTcl::_marshallDict list {
    set result [dict create]
    foreach [list key value] $list {
        dict lappend result $key $value
    }
    return $result
}
##
# Return a JSON String array from a list of Tcl strings
#
# @param strings - list of strings
# @return JSON encoded array of strings.
#
#
proc ::SpecTcl::_jsonStringArray strings {
    set stringArray [list]

    foreach string $strings {
	lappend stringArray [json::write string $string]
    }
    return [json::write array {*}$stringArray]
}
##
# Return a JSON return object:
# @param status - JSON encoding of the status code {string}
# @param detail - JSON encoding of the detail object
#
# @return return object.
#
proc ::SpecTcl::_returnObject {{status OK} {detail ""}} {
    if {($status eq "OK") && ($detail eq "")} {
	return [json::write object status [json::write string $status]]
    }
    return [json::write object \
		status [json::write string $status] detail $detail]
}
##
# Marshall a set of xcoord(i)/ycoord(i) parameters into a set of x/y pairs.
# This is done so that the order in which the query params get marshalled into the'
# method parameters don't matter.
#
# @param dict - query parameter dictionary. We are interested in keys like xcoord(*) and ycoord(*)
#
# @return list of {x y} sublists.
#
proc ::SpecTcl::_marshallXYPoints {dict} {
    set xcoords [dict filter $dict key xcoord(*)]
    set ycoords [dict filter $dict key ycoord(*)]

    # Turn these dicts into xcoord/ycoord arrays:

    array set xcoord [list]
    array set ycoord [list]
    dict for [list key value] $xcoords  {
	set $key $value
    }
    dict for [list key value] $ycoords  {
	set $key $value
    }

    #  Indices number from 0:
    
    set index 0
    set coords [list]


    while {1} {
	if {([array names xcoord $index] ne "") && ([array names ycoord $index] ne "")} {
	    lappend coords [list $xcoord($index) $ycoord($index)] 
	} else {
	    break;		# exit loop when we run out of one or the other coordinates.
	}
	incr index
    }

    return $coords
}
