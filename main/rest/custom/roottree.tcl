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
# @file   roottree.tcl
# @brief  Provide access to the roottree command.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write
Direct_Url /spectcl/roottree  SpecTcl_roottree

##
# SpecTcl_roottree/create
#  Make a new root tree:
#
# @param tree  - name of the tree.
# @param parameter - Glob patterns of pareameters.
# @param gate      - optional gate to conditionalize the events that get booked
#                    into the tree.
#
proc SpecTcl_roottree/create {args} {
    set ::SpecTcl_roottree/create application/json
    
    set qdict [SpecTcl::_marshallDict $args]

    # Check for required parameters:
    
    if {![dict exists $qdict tree]} {
        return {SpecTcl::_returnObject "Missing required 'tree' parameter"}
    }
    if {![dict exists $qdict parameter]} {
        return [SpecTcl::_returnObject "Missing 'parameter' pattern parameter(s)"]
    }
    
    # Construct the root tree command:
    
    set command [list roottree create \
        [dict get $qdict tree] [dict get $qdict parameter] \
    ]
    if {[dict exists $qdict gate]} {
        lappend command [dict get $qdict gate]
    }
    #
    #  Now try to execute the command:
    #
    set status [catch {
        {*}$command
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject "'roottree create' command failed" \
            [json::write string $msg]                                     \
        ]
    }
    
    return [SpecTcl::_returnObject OK]
    
}
##
# SpecTcl_roottree/delete
#   Delete the specified tree name.
#
# @param tree - the tree to delete.
#
proc SpecTcl_roottree/delete {tree} {
    set ::SpecTcl_roottree/delete application/json
    
    set status [catch {
        roottree delete $tree
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject "'roottree delete' command failed" \
            [json::write string $msg]    \
        ]
    }
    return [SpecTcl::_returnObject OK]      
}
##
# SpecTcl_roottree/list
#   Produce a listing of the root trees.
#
# @param pattern - pattern with glob characters that indicates the
#                 names of the trees to list. Defaults to * if not supplied which
#                   matches all trees.
#
proc SpecTcl_roottree/list {{pattern *}} {
    set ::SpecTcl_roottree/list application/json
    
    set status [catch {
        roottree list $pattern
    } info]
    if {$status} {
        return [SpecTcl::_returnObject "'roottree list' command failed" \
            [json::write string $info]
        ]
    }
    
    set resultList [list]
    foreach treeinfo $info {
        set tree [lindex $treeinfo 0]
        set params [::SpecTcl::_jsonStringArray [lindex $treeinfo 1]]
        set gate   [lindex $treeinfo 2]
        lappend resultList [json::write object                     \
            tree [json::write string $tree]                        \
            parameters [json::write array {*}$params]              \
            gate [json::write string $gate]                        \
        ]
    }
    return [SpecTcl::_returnObject OK [json::write array {*}$resultList]]
}