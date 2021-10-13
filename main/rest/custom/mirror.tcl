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
# @file   mirror.tcl
# @brief  Provide access to the SpecTcl mirror command.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/mirror  SpecTcl_mirror

##
# SpecTcl_mirror
#   Just do a mirror command to get a list of all of the mirrors that are
#   currently active.  The resulting detail is an array of objects with
#   host and shmkey attributes.
#
proc SpecTcl_mirror {{pattern *}} {
    set ::SpecTcl_mirror application/json
    
    set result [mirror list $pattern]
    set listing [list]
    foreach item $result {
        lappend listing [json::write object {*}$item]
    }
    
    SpecTcl::_returnObject OK [json::write array {*}$listing]
    
}