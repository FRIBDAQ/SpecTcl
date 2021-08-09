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
# @file   filter.tcl
# @brief  REST support for the SpecTcl 'filter' command
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write

Direct_Url /spectcl/filter  SpecTcl_filter

##
# SpecTcl_filter/new
#
#   Create a new filter.
#
# @param name - name of the new filter.
# @param gate - Name of the gate that determines which events are written.
# @param parameter...- parameters to output to the filter.
# @note Since there can be many occurences of parameter, we'll really use the
#       args parameter.
#
proc SpecTcl_filter/new {args} {
    set ::SpecTcl_filter/new application/json
    
    set queryParams [::SpecTcl::_marshallDict $args]
    
    # Ensure the user supplied all query parameters:
    
    if {![dict exists $queryParams name]} {
        return [SpecTcl::_returnObject \
            "Missing required query parameter: " \
            [json::write string name]            \
        ]
    }
    if {![dict exists $queryParams gate]} {
        return [SpecTcl::_returnObject \
            "Missing required query parameter: " \
            [json::write string gate]            \
        ]
    }
    if {![dict exists $queryParams parameter]} {
        [SpecTcl::_returnObject \
            "Missing required query parameter: " \
            [json::write string parameter]       \
        ]
    }
    #  Now try to make the filter:
    
    set status [catch {
        filter -new     \
            [dict get $queryParams name]          \
            [dict get $queryParams gate]          \
            [dict get $queryParams parameter]     \
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject \
            "'filter -new' command failed"             \
            [json::write string $msg]                  \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_filter/delete
#    Delete a filter.
#
# @param name - name of the fiter to delete.
#
proc SpecTcl_filter/delete {name} {
    set ::SpecTcl_filter/delete application/json
    
    set status [catch {
        filter -delete $name    
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject                 \
            "'filter -delete' command failed"           \
            [json::write string $msg]                   \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_filter/enable
#   Enables an existing filter.
#
# @param name - Name of the filter to enable.
#
proc SpecTcl_filter/enable {name} {
    set ::SpecTcl_filter/enable application/json
    
    set status [catch {
        filter -enable $name
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject               \
            "'filter -enable' command failed"        \
            [json::write string $mgs]                \
        ]
    }
    
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_filter/disable
#
#   Disable a named filter.
#
# @param name -- name of the filter to disable.
#
proc SpecTcl_filter/disable {name} {
    set ::SpecTcl_filter/disable application/json
    
    set status [catch {
        filter -disable $name
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject                                      \
            "'filter -disable' command failed"                              \
            [json::write string $msg]
        ]
    }
    
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_filter/regate
#   Change the gate on a filter.
#
# @param name - name of the filter.
# @param gate - new gate name.
#
proc SpecTcl_filter/regate {name gate} {
    set ::SpecTcl_filter/regate application/json
    
    set status [catch {
        filter -regate $name $gate
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject                        \
            "'filter -regate' command failed"                 \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}

##
# SpecTcl_filter/file
#
# Sets the output file to which a filter will write data.
#
# @param name - name of the filter.
# @param file - Name of the output file. Note that this is interpreted in the
#               context of this server and must be writable by this process, not
#               the client.  As such it is recommended to use full path names.
#
proc SpecTcl_filter/file {name file} {
    set  ::SpecTcl_filter/file application/json
    
    set status [catch {
        filter -file $file $name
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject     \
            "'filter -file' command failed"   \
            [json::write string $msg]        \
        ]
    }
    return [SpecTcl::_returnObject OK]
}
##
# SpecTcl_filter/list
#    List the filter definitions.
#
# @param pattern - optional GLOB pattern that determines the
#                  filter listed (only those with matching names).
#                  if not provided, * is used which matches all filters.
# @return the detail part of the JSON object returned is an array of objects.
#                  Each object describefs a single filter and has the following
#                  attributes:
#              - name - filter name.
#              - gate - name of the gate applied to the filter.
#              - file - name of the output file (may be an empty string).
#              - parameters - array of parameter names written to the filter.
#              - enabled  - String containing 'enabled' or 'disabled'.
#              - format   - Filter format e.g. 'xdr'.
#
proc SpecTcl_filter/list {{pattern *}} {
    set ::SpecTcl_filter/list application/json
    
    set status [catch {
        filter -list $pattern
    } info]
    
    if {$status} {
        return [SpecTcl::_returnObject                 \
            "'filter -list' command failed"            \
            [json::write string $info]                 \
        ]
    }
    set result [list]
    foreach filter $info {
        
        # Need to properly JSON write the parameters:
        
        set rawParams [lindex $filter 3]
        set paramArray [list]
        foreach param $rawParams {
            lappend paramArray [json::write string $param]
        }
        
        lappend result [json::write object                         \
            name [json::write string [lindex $filter 0]]            \
            gate [json::write string [lindex $filter 1]]          \
            file [json::write string [lindex $filter 2]]          \
            parameters [json::write array {*}$paramArray]         \
            enabled [json::write string [lindex $filter 4] ]      \
            format [json::write string [lindex $filter 5]]        \
        ]
    }
    
    return [SpecTcl::_returnObject OK [json::write array {*}$result]]
}
##
# SpecTcl_filter/format
#   Changes the format of a filter's output.
# @param name - filter name
# @param format - filter format specifier.
#
proc SpecTcl_filter/format {name format} {
    set ::SpecTcl_filter/format application/json
    
    set status [catch {
        filter -format $name $format
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject                 \
            "'filter -format' command failed"          \
            [json::write string $msg]                  \
        ]
    }
    
    return [SpecTcl::_returnObject OK]
}
