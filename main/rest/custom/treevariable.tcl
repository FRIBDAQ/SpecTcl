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
# @file treevariable.tcl
# @brief REST interface to treevariables.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write

Direct_Url /spectcl/treevariable SpecTcl_treevariable

##
# SpecTcl_treevariable/list
#    List the tree variables and their properties.
#
proc SpecTcl_treevariable/list {} {
    set ::SpecTcl_treevariable/list application/json
    
    set status [catch {
        treevariable -list
    } info]
    if {$status} {
        return [SpecTcl::_returnObject "'treevariable -list' failed: " \
            [json::write string $info]                              \
        ]
    }
    set result [list]
    foreach var $info {
        lappend result [json::write object                            \
            name [json::write string [lindex $var 0]]                 \
            value [lindex $var 1]                                     \
            units [json::write string [lindex $var 2]]               \
        ]
    }
    SpecTcl::_returnObject "OK" [json::write array {*}$result]
}
##
# SpecTcl_treevariable/set
#   Set a new value and units for a tree variable
#
# @param name - name of the variable
# @param value - new value.
# @param units - new units.
# No useful information is returned.
#
proc SpecTcl_treevariable/set {name value units} {
    set ::SpecTcl_treevariable/set application/json
    
    set status [catch {
        treevariable -set $name $value $units
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'treevariable -set' failed" \
            [json::write string $msg]
        ]
    }
    SpecTcl::_returnObject OK
}

##
# SpecTcl_treevariable/check
#   Return the changed flag.
# @param name
#
proc SpecTcl_treevariable/check {name} {

    set ::SpecTcl_treevariable/check application/json
    
    set status [catch {
        treevariable -check $name
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject                        \
            "'treevariable -check' failed" [json::write string $msg]                      \
        ]
    }
    SpecTcl::_returnObject OK $msg
}
##
# SpecTcl_treevariable/setchanged
#   Set the changed flag of a variable.
#
# @param name - name to set.
#
proc SpecTcl_treevariable/setchanged {name} {
    set ::SpecTcl_treevariable/setchanged application/json
    
    set status [catch {
        treevariable -setchanged $name
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject                      \
            "'treevariable -setchanged' command failed" [json::write string $msg]                    \
        ]
    }
    SpecTcl::_returnObject OK
}
##
# SpecTcl_treevariable/firetraces
#    Fire any traces set on a treevariable.
#
# @param name pattern - optional pattern that confines the set of traces by
#                   those that match the glob pattern.
#
proc SpecTcl_treevariable/firetraces {{pattern *}} {
    set ::SpecTcl_treevariable/firetraces application/json
    
    set status [catch {
        treevariable -firetraces $pattern
    } msg]
    
    if {$status} {
        return [::SpecTcl::_returnObject \
            "'treevariable -firetraces failed: " [json::write string $msg] \
        ]
    }
    SpecTcl::_returnObject OK
}
