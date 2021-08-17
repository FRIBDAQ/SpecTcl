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
# @file script.tcl
# @brief Allow an arbitrary script to be run inside of SpecTcl via REST
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write
Direct_Url /spectcl/script SpecTcl_script

##
# SpecTcl_script
#    Invoke a script inside of SpecTcl given a command string.
# @param command - string containing the script to run.
#                  This will be run as {*}$command which breaks it down
#                  into list elements.
# @return - the script return value in detail.
#
proc SpecTcl_script {command} {
    
    set ::SpecTcl_script application/json
    
    set status [catch {
        uplevel #0 $command
    } result]
    if {$status} {
        return [SpecTcl::_returnObject ERROR [json::write string $result]]
    }
    return [SpecTcl::_returnObject OK [json::write string $result]]
}