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
# @file  ringformat
# @brief Companion to attach -format ring  - sets the format of the ring data.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/ringformat SpecTcl_ringformat

##
# SpecTcl_ringformat
#    Set the ring buffer format when attach -format ring is used.
#
# @param major - major version.
# @param minor - Minor version.
#
proc SpecTcl_ringformat {major {minor 0}} {
    set ::SpecTcl_ringformat application/json
    
    set status [catch {
        ringformat $major.$minor
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject "'ringformat' command failed"      \
            [json::write string $msg]                                     \
        ]
    }
    return [SpecTcl::_returnObject OK]
}