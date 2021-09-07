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
# @file   analyze
# @brief  Support start/stop analysis.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write

Direct_Url /spectcl/analyze  SpecTcl_analyze

##
# SpecTcl_analyze/start
#   Start analyzing data from the data source.
#
proc SpecTcl_analyze/start { } {
    set ::SpecTcl_analyze/start application/json
    
    set status [catch {
        start
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'start' command failed" \
                [json::write string $msg]                         \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_analyze/stop
#   Stop analyzing data from the data source.
#
proc SpecTcl_analyze/stop { } {
    set ::SpecTcl_analyze/stop application/json
    
    set status [catch {
        stop
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'stop' command failed" \
                [json::write string $msg]                         \
        ]
    }
    return [SpecTcl::_returnObject OK]
}