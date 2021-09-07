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
# @file unbind.tcl
# @brief Provide access to the SpecTcl unbind command.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write
Direct_Url /spectcl/unbind SpecTcl_unbind

##
# To easily support all forms of unbind we're going to separate them
# out into more than one URL.

##
# SpecTcl_unbind/byname
#    Remove bindings given one or more spectrum names.
#
# @param name Names to unbind.
#
proc SpecTcl_unbind/byname {args} {
    set ::SpecTcl_unbind/byname application/json
    
    set qparams [SpecTcl::_marshallDict $args]
    if {![dict exists $qparams name]} {
        return [SpecTcl::_returnObject "'name' is a required query parameter"]
    }
    set status [catch {
        unbind {*}[dict get $qparams name]
    } msg]
        
    if {$status} {
        return [SpecTcl::_returnObject "'unbind' command failed"  \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_unbind/byid
#
#  Unbind using a set of ids'
# @param id - params containing ids to unbind.
#
proc SpecTcl_unbind/byid {args} {
    set ::SpecTcl_unbind/byid application/json
    set qparams [SpecTcl::_marshallDict $args]
    
    if {![dict exists $qparams id]} {
        return [SpecTcl::_returnObject "'id' is a required query parameter"]
    }
    set status [catch {
        unbind -id {*}[dict get $qparams id]
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject "'unbind -id' command failed" \
            [json::write string $msg]
        ]
    }
    
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_unbind/all
#   Unbind all spectra.
#
proc SpecTcl_unbind/all {} {
    set ::SpecTcl_unbind/all application/json
    
    set status [catch {
        unbind -all
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'unbind -all' failed"    \
            [json::write string $msg]                            \
        ]
    }
    return [SpecTcl::_returnObject OK]
}