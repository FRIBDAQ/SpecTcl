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
# @file evbunpack.tcl
# @brief REST access to evbunpack.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write
Direct_Url /spectcl/evbunpack SpecTcl_evbunpack

##
# SpecTcl_evbunpack/create
#    Create a new unpacker for event built data.
#    This unpacker is registered with the pipeline manager.
# @param name - name of the  unpacker.
# @param frequency - Clock frequency of the timestamp.  This is used in creating
#                    the diagnostic tree parameters.  This is a floatig point
#                    value in MHz units.
# @param basename - Tree parameter base name for the diagnostic parameters
#                   created by this event processor.
#
proc SpecTcl_evbunpack/create {name frequency basename} {
    set ::SpecTcl_evbunpack/create application/json
    
    set result [catch {
        evbunpack create $name $frequency $basename
    } msg]
    if {$result} {
        return [SpecTcl::_returnObject "'evbunpack create' command failed" \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_evbunpack/add
#     Add a processor for source id.  The processor is an existing
#     event processing pipeline.
#
# @param evpname - name of the event processor  to add to.
# @param source  - Source id the processor works on.
# @param pipe    - Pipeline to process the  source.
#
proc SpecTcl_evbunpack/add {name source pipe} {
    set ::SpecTcl_evbunpack/add application/json
    
    set result [catch {
        evbunpack addprocessor $name $source $pipe
    } msg]
    if {$result} {
        return [SpecTcl::_returnObject \
            "'evbunpack 'addprocessor' command failed"              \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_evbunpack/list
#
#   list the names of the event processors that are created by this
#   command.
#
# @param pattern - defaults to * - only names matching this pattern are returned.
#
proc SpecTcl_evbunpack/list {{pattern *}} {
    set ::SpecTcl_evbunpack/list application/json
    
    set result [catch {
        evbunpack list $pattern
    } info]
    if {$result} {
        return [SpecTcl::_returnObject "'evbunpack list' command failed" \
            [json::write string $info]
        ]
    }
    return [SpecTcl::_returnObject OK [SpecTcl::_jsonStringArray $info]]
}