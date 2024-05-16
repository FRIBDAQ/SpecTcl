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
# @file attach.tcl
# @brief REST service to attach SpecTcl to a data source.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write

Direct_Url /spectcl/attach SpecTcl_Attach

namespace eval attach {

}
proc attach::_valueOrDefault {d key default} {
    if {[dict exists $d $key]} {
        return [dict get $d $key]
    } else {
        return $default
    }
}

##
# SpecTcl_Attach/attach
#    REST server wrapper for the attach command used to attach SpecTcl
#    to a new data source.  Note that it's still necessary to start analysis.
#
# @param type   - Type of data source : file or pipe.
# @param source - Source string apropriate to the type.
# @param size   - optional blocking size defaults to 8192.
# @param format - optional data format, defaults to ring
# @note we let the attach command validate the bits and pieces it's passed.
#      that allows us to not modify code as attach may change in the future.
#
# This only returns errors, it does not return any details on success.
# See, however SpecTcl_Attach/list.
#
proc SpecTcl_Attach/attach {type source {size 8192} {format {ring}}} {
    set ::SpecTcl_Attach/attach application/json
    set typesw -$type
    
    # Note that source can be many words.
    
    set status [catch {
        attach $typesw -size $size -format $format {*}$source
    } msg]
    if {$status} {
        SpecTcl::_returnObject "attach command failed" [json::write string $msg]
    } else {
        SpecTcl::_returnObject OK
    }
}
##
# SpecTcl_Attach/list
#    return the attachment string as the details:
#
proc SpecTcl_Attach/list {} {
    set ::SpecTcl_Attach/list application/json
    SpecTcl::_returnObject OK [json::write string [attach -list]]
}
#   Added for issuu #94 - Support REST initiated clustser processing.
##
#   SpecTcl_Attach/cluster
#
#  @param file - cluster file - the file with a list of run
#  @param format - defaults to ring11
#  @param size   - Defaults to 8192
#
# @note Cluster file processing is started via the event loop (after).
#       We do check the readability of the file but not
#       its formant nor the existence/readaibility of the run files
#       within.  It is therefore possible that cluster file processing
#       will appear (to the client) to start normally but then fail.
#
proc SpecTcl_Attach/cluster {args} {
    set ::SpecTcl_Attach/cluster application/json
    set queryParams [::SpecTcl::_marshallDict $args]
    
    #  file is required..the others default:

    set missingKey [::SpecTcl::_missingKey $queryParams [list file]]
    if {$missingKey ne ""} {
        return [::SpecTcl::_returnObject "Missing required parameter" [json::write string $missingKey]]
    }

    set file [dict get $queryParams file]
    set format [::attach::_valueOrDefault $queryParams format ring11]
    set size [::attach::_valueOrDefault $queryParams size 8192]

    #  Can't start procesing if we alread are processing:

    if {[::datasource::processingCluster]} {
        return [::SpecTcl::_returnObject \
            "Cluster file processing in progress" \
            [json::write string "Either wait until done or abort"] \
        ]
    }

    if {![file readable $file]} {
        return [::SpecTcl::_returnObject \
            "Cannot open cluster file"  \
            [json::write string $file]   \
        ]
    }
    if {[catch {open $file r} fd]} {
        return [::SpecTcl::_returnObject \
            "Unable to open readable clustser file $file" \
            [json::write string $fd]                      \
        ]
    }
    after 1 ::datasource::startClusterFile $fd $format $size


    return [::SpecTcl::_returnObject]


}

##
#  SpecTcl_Attach/stop_cluster
#
#    If cluster processing is active, stops it.
#
#
proc SpecTcl_Attach/cluster_stop {args} {
    set ::SpecTcl_Attach/clustesr_stop application/json

    if {![::datasource::processingCluster]} {
        return [::SpecTcl::_returnObject \
            "Cluster processing is not active" \
            [json::write string ""]             \
        ]
    }
    datasource::stopCluster

    return [::SpecTcl::_returnObject]
}


    
