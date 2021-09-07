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
# @file   apply.tcl
# @brief  Provide REST interface to apply command.
# @author Ron Fox <fox@nscl.msu.edu>

package require json::write

Direct_Url /spectcl/apply SpecTcl_Apply

##
# SpecTcl_Apply/apply
#
# Create a new gate application.
# The gate and spectrum parameters are mandatory.
#
# @param  gate - Gate to apply.
# @param spectrum - spectrum to apply it to.
# @note the detail of the result is empty.
#
proc SpecTcl_Apply/apply {{gate ""}  {spectrum ""} } {
    set ::SpecTcl_Apply/apply application/json
    if {$gate eq ""} {
        
        return [::SpecTcl::_returnObject "Missing parameter" [json::write string gate]]
    }
    if {$spectrum eq ""} {
        return [::SpecTcl::_returnObject "Missing  parameter" [json::write string spectrum]]

    }
    
    set status [catch {apply $gate $spectrum} msg]
    if {$status} {
        return [::SpecTcl::_returnObject "'apply' command failed" [json::write string $msg]]
    } else {
        return [SpecTcl::_returnObject]
    }
}
##
# SpecTcl_Apply/list
#
# list gate applications.  The details part of the gate contains an array
#  Each element of that array is an object with the
# attributes:
#    *   spectrum - name of the spectrum.
#    *   gate     - name of the applied gate.
#
#
#  The optional pattern query parameter is a glob pattern that restricts
#  the set of spectra listed.  If not provided it's as if * was the
#  value and all spectra are listed.
# @note in this release if you want more than the gate name, it's necessary
#       to ask for the gate definition through the spectcl/gate domain.
#
#
proc SpecTcl_Apply/list {{pattern *}} {
    set ::SpecTcl_Apply/list application/json
    set applications [apply -list $pattern]
    set elements [list]
    foreach application $applications {
        set spectrum [lindex $application 0]
        set gate     [lindex [lindex $application 1] 0]
        lappend elements [json::write object                  \
            spectrum [json::write string $spectrum]          \
            gate [json::write string $gate]                     \
        ]
    }
    set details [json::write array {*}$elements]
    return [SpecTcl::_returnObject OK $details]
}
