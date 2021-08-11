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
# @file   pseudo.tcl
# @brief provide access to the SpecTcl pseudo command.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/pseudo SpecTcl_pseudo


##
# SpecTcl_pseudo/create
#
#  Create a new pseudo parameter.  The pseudo binds a  set of existing
#   parameters into a new existing parameter via a Tcl script that
#   computes the result.
#
#  See the SpecTcl pseudo command documentation.
#
# @param psuedo - name of the new pseudo parameter
# @param parameter - names (multiple occurences)  of the parameters
#                  pseudo depends on
# @param computation - the Tcl procedure body that computes the psueudo
#                  from the parameters.
#
proc SpecTcl_pseudo/create {args} {
    set ::SpecTcl_pseudo/create application/json
    
    set pdict [::SpecTcl::_marshallDict $args]
    
    # All parameters are required, we just use the dict to deal with
    # multiple occurences.
    
    if {![dict exists $pdict pseudo]} {
        return [SpecTcl::_returnObject "Missing required 'pseudo' parameter"]
    }
    if {![dict exists $pdict parameter]} {
        return [SpecTcl::_returnObject "Missing required 'parameter' parameter"]
    }
    if {![dict exists $pdict computation]} {
        return [SpecTcl::_returnObject "Missing required 'computation' parameter"]
    }
    
    set name [dict get $pdict pseudo]
    set parameters [dict get $pdict parameter]
    set body {*}[dict get $pdict computation];   # The dict get brackets one more time :-()
    
    set result [catch {
        pseudo $name $parameters $body
        
    } msg]
    if {$result} {
        return [SpecTcl::_returnObject "'pseudo' command failed" \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_pseudo/list
#    List the pseudo definitions.
# @param pattern - optional pattern that defaults to *.  Only
#        pseudo parameters with name matching the pattern, which can contain
#        glob wildcards are listed.
# @return the detail part of the JSON object returned to the client is an array
#      of objects.  Each object describes a psuedo parameter andhas
#      the following attributes:
#      -   name - name of the pseudo parameter.
#      -   parameters - array of parameter names.
#      -   computation - the computation performed to generate the pseudo.
#
proc SpecTcl_pseudo/list {{pattern *}} {
    set ::SpecTcl_pseudo/list application/json
    
    set status [catch {
        pseudo -list $pattern
    } info]
    if {$status} {
        return [SpecTcl::_returnObject "'pseudo -list' command failed"
            [json::write string $info]
        ]
    }
    set resultList [list]
    foreach param $info {
        set name [lindex $param 0]
        set parameter [lindex $param 1]
        set body [lindex $param 2]
        lappend resultList [json::write object                        \
            name [json::write string $name]                           \
            parameters [::SpecTcl::_jsonStringArray $parameter]      \
            computation [json::write string $body]                    \
        ]
    }
    
    return [SpecTcl::_returnObject OK [json::write array {*}$resultList]]
}
##
# SpecTcl_pseudo/delete
#    Delet a named psuedo.
# @param name - name of the pseudo to delete.
#
proc SpecTcl_pseudo/delete {name} {
    set ::SpecTcl_pseudo/delete application/json
    
    set status [catch {
        pseudo -delete $name
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'pseudo -delete' command failed"\
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}