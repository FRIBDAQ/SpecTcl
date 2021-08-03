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
# @file   specstats.tcl
# @brief  Provide REST access to the specstats command.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/specstats SpecTcl_specstats


##
# SpecTcl_specstats
#    Provides spectrum statistics.
# @param pattern - glob pattern to restrict the set of spectrum names.
#
# The detail returned is an array of objects with name, underflows and overflows
# as attributes both underflows and overflows are arrays of counters.
#
proc SpecTcl_specstats {{pattern *}} {
    set ::SpecTcl_specstats application/json
    set status [catch {
        specstats $pattern
    } info]
    
    if {$status} {
        SpecTcl::_returnObject "'specstats' command failed: " [json::write string $info]
    } else {
        set result [list]
        foreach statDict $info {
            set name [dict get $statDict name]
            set underflows [dict get $statDict underflows]
            set overflows [dict get $statDict overflows]
            lappend result [json::write object                                       \
                name [json::write string $name]                               \
                underflows [json::write array {*}$underflows]                  \
                overflows [json::write array {*}$overflows]
            ]
        }
        SpecTcl::_returnObject OK [json::write array {*}$result]
    }
}