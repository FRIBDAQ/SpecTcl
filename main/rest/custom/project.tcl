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
# @file project.tcl
# @brief Provide REST support for the project command.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write

Direct_Url /spectcl/project SpecTcl_project

##
# SpcTcl_project
#   Project a spectrum making a new spectrum.
#
# @param snapshot - boolean true or false specifying if the result is a snapshot.
# @param source   - Source Spectrum name.
# @param newname  - Name of new spectrum.
# @param direction - x|y the projection direction.
# @param contour  - Optional contour within which the projection is done.
#                   if not provided the entire spectrum is projected.#
proc SpecTcl_project {snapshot source newname direction {contour ""}} {
    set ::SpecTcl_project application/json
    if {$snapshot} {
        set sn -snapshot
    } else {
        set sn -nosnapshot
    }
    set command [list project $sn $source $newname $direction]
    if {$contour ne ""} {
        lappend command $contour
    }
    
    set status [catch {
        {*}$command
    } msg]
    
    if {$status} {
        SpecTcl::_returnObject "'project' command failed: " [json::write string $msg]
    } else {
        SpecTcl::_returnObject OK
    }
}