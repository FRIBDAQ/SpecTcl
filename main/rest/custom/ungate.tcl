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
# @file  ungate.tcl
# @brief Un gate one or more spectra (the spectra become gated on the buit-in
#        true gate).
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/ungate SpecTcl_ungate

##
# SpecTcl_ungate
#   Ungate the named spectra.
#
# @param name - the names of spectra to ungate.
#
proc SpecTcl_ungate {args} {
    set ::SpecTcl_ungate application/json
    set qdict [SpecTcl::_marshallDict $args]
    
    if {![dict exists $qdict name]} {
        return [SpecTcl::_returnObject "The 'name' query parameter is required"]
    }
    set status [catch {
        ungate {*}[dict get $qdict name]
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject "'ungate' command failed"    \
            [json::write string $msg]                                      \
        ]
    }
    return [SpecTcl::_returnObject OK]
}