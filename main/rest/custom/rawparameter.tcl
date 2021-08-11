#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2014.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Authors:
#             Ron Fox
#             Giordano Cerriza
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321


##
# @file rawparameter.tcl
# @brief Access to the 'parmeter' command (note that parameter.tcl accesses
#        some hellish combination of parameter/treeparameter access).
# @author Ron Fox <fox@nscl.msu.edu>
#
#

package require json::write

Direct_Url /spectcl/rawparameter SpecTcl_rawparameter

##
# SpecTcl_rawparameter/new
#
#    Creates a new parameter.  Note that the form of the parameter
#    command is quite flexible allowing for resolution in bits
#    resolution in bits with low/high suggested spectrum limits and
#    the units.
#
# @param name - parameter name (required)
# @param number - Parameter number (required)
# @param resolution - Bits of parameter resolution (optional).
# @param low, high  - Suggested spectrum limits (optional but resolution is required as are units)
# @param units      - Parameter units of measure (optional)
#
proc SpecTcl_rawparameter/new {args} {
    set ::SpecTcl_rawparameter/new application/json
    
    puts $args
    set qdict [SpecTcl::_marshallDict $args]
    puts $qdict
    
    # Name and number are required:
    
    if {(![dict exists $qdict name]) || (![dict exists $qdict number])} {
        return [SpecTcl::_returnObject "Creating a new parameter requires at least a parameter name and number"]
    }
    
    set name [dict get $qdict name]
    set number [dict get $qdict number]
    
    # Remove those keys so we can see only what we have left:
    
    dict unset qdict name
    dict unset qdict number
    
    puts $qdict
    
    set keys [dict keys $qdict]
    set command [list]
    if {[llength $keys] == 0} {
        set command [list parameter -new $name $number]
    } elseif {[dict exists $qdict resolution]}  {
        
        # May or may not have high low units:
      
        if {[dict exists $qdict low]} {
            if {(![dict exists $qdict high]) || (![dict exists $qdict units])} {
                return [SpecTcl::_returnObject "low, requires high and units"]
            }
            set resolution [dict get $qdict resolution]
            set low        [dict get $qdict low]
            set high       [dict get $qdict high]
            set units      [dict get $qdict units]
            
            set command [list parameter -new $name $number \
                $resolution [list $low $high $units]       \
            ]
            
        } else {
            set command [list parameter -new $name $number [dict get $qdict resolution]]
        }
        
        
    } elseif {[dict exists $qdict units]} {
        set command [list parameter -new $name $number [dict get $qdict units]]
    }
    
    if {$command eq [list]} {
        return [SpecTcl::_returnObject "Could not determine proper parameter command"]
    }
    
    set status [catch {
        {*}$command
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject                                  \
            "'parameter -new' command failed"                           \
            [json::write string $msg]                                   \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_rawparameter/delete
#  Delete a parameter given either it's name or id.
#
# @param name -parameter name.
# @param id  - parameter id.
# @note ambiguity (both name and id given) is resolved in favor of name.
#
proc SpecTcl_rawparameter/delete {args} {
    set ::SpecTcl_rawparameter/delete application/json
    set qdict [SpecTcl::_marshallDict $args]
    
    if {[dict exists $qdict name]} {
        set command [list parameter -delete [dict get $qdict name]]
    } elseif {[dict exists $qdict id] } {
        set command [list parameter -delete -id [dict get $qdict id]]
    } else {
        return [SpecTcl::_returnObject "Either a name or id is required to delete a parameter"]
    }

    set status [catch {
        {*}$command
    } msg]

    if {$status} {
        return [SpecTcl::_returnObject                  \
            "'parameter -delete' command failed"       \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}

##
# SpcTcl_rawparameter/list
#    list either parameter that match a pattern or a single parameter by id
#
# @param pattern - glob pattern to match.
# @param id      -  id
# @return the detail is an array of JSON Objects that contain some mandatory attributes
#         and some optional.
#    - name - parameter name (mandatory)
#    - id   - parameter id/number
#    - resolution Number of bits of parameter resolution (optional).
#    - low   - Parameter low limit (optional)
#    - high  - parameter high limit (optional).
#    - units - parameter units (optional
#
proc SpecTcl_rawparameter/list {args} {
    set ::pcTcl_rawparameter/list application/json
    
    set qdict [SpecTcl::_marshallDict $args]
    if {[dict exists $qdict pattern]} {
        set info [parameter -list [dict get $qdict pattern]]
    } elseif {[dict exists $qdict id]} {
        set info [list [parameter -list -id [dict get $qdict id]]]
    } else {
        return [SpecTcl::_returnObject "Either pattern or id is required."]
    }
    #  Now we can process the parameter listing.  Note that each
    #  parameter has all fields but the empty (blank) ones are 'not present'
    #
    set resultList [list]
    foreach param $info {
        set item [dict create                                   \
            name [json::write string [lindex $param 0]]   \
            id [lindex $param 1]     \
        ]
        set resolution [lindex $param 2]
        set advanced [lindex $param 3]
        set low [lindex $advanced 0]
        set high [lindex $advanced 1]
        set units [json::write string [lindex $advanced 2]]
        
        # This is a bit sneaky.  It relies on the fact that the values
        # of each potential key are in a variable named the same as that key.
        # This allows us to do this loop below rather than check each value
        # individually.
        
        foreach key [list resolution low high units] {
            set value [set $key];     # sneaky me.
            
            if {$value ne ""} {
                dict set item $key $value
            }
        }
        lappend resultList [json::write object {*}$item]
    }
    
    return [SpecTcl::_returnObject OK [json::write array {*}$resultList]]
}