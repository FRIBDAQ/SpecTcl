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
# @file waveform.tcl
# @brief provide a /spectcl/waveform domain.
# @author Ron Fox

package require json::write
Direct_Url /spectcl/waveform SpecTcl_waveform

namespace eval waveform {
    # Local proc to turn a definition into a JSON object
    proc _defToJson def {
        #  First make an array out of the metadata:

        set metadata [dict get $def metadata]
        set metadatalist [list]
        dict for {key value} $metadata {
            lappend metadatalist [json::write object \
                name [json::write string $key]      \
                value [json::write string $value]   \
            ]
        }
        # Now the final object:

        return [json::write object \
            name [json::write string [dict get $def name]]        \
            samples [json::write string [dict get $def samples]]  \
            metadata [json::write array {*}$metadatalist]         \
        ]
    }
    proc _traceToJson trace {
        return [json::write object \
            name [json::write string[lindex $trace 0]] \
            samples [json::write array {*}[lindex $trace 1]] \
        ]
    }
}
##
# SpecTcl_waveform/create
#    Creates a new waveform;
# Query params:
#    @param name - name of the new waveform
#    @param samples - number of samples (must be integer.)
# Result:
#   On success, the detail is empty.
#
proc SpecTcl_waveform/create {name samples} {
    set ::SpecTcl_waveform/create application/json;  # All return JSON.
    set status [catch {
        waveform create $name $samples
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'waveform create' failed" \
            [json::write string $msg]                          \
        ]
    } 
    SpecTcl::_returnObject OK
}
##
#  SpecTcl_waveform/list
#     List the waveform definitions that have names which  match the optional pattern
#     query parameter as a glob pattern.  If there is no pattern parameter,
#     it defaults to "*" which matches all waveform names.
#
#   The result is an array of waveform definitions where each definition consists of the
#   following attributes:
#
#  - name - the name of the waveform.
#  - samples - the number of samples in the waveform.
#  - metadata - An array of name value objects for each metadata item.
proc SpecTcl_waveform/list {{pattern *}} {
    set ::SpecTcl_waveform/list application/json;  # All return JSON.
    set listing [waveform list $pattern]

    set result [list]
    foreach definition $listing {
        lappend result [waveform::_defToJson $definition]
    }
    SpecTcl::_returnObject "OK" [json::write array {*}$result]
}

##
#  SpecTcl_waveform/get
#    Get the waveform values from a single waveform.
#    Note that since this is could be MPI mode the returned
#    thing is an array of objects with the attributes:
# - name - name of the waveform
# - samples - an array of sample points.
#
proc SpecTcl_waveform/get {name} {
    set ::SpecTcl_waveform/get application/json;  # All return JSON.
    set status [catch {
        waveform get $name
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'waveform get' failed" \
            [json::write string $msg]                        \
        ]
    }
    set result [list]
    foreach trace $msg {
        lappend result [_wavform::_traceToJson $trace]
    }
    SpecTcl::_returnObject OK [json::write array {*}$result]
}
##
# SpecTcl_waveform/metadata/get
#    Return metadata values.
# Query params: 
#   @param name - name of the waveform
#   @param key  - (optional) key of the metadata item to get.
# If key is not specified, all metadata items are returned.
# Result:
#   An array of name value objects for each metadata item.
#
proc SpecTcl_waveform/metadata/get {name {key }} {
    set ::SpecTcl_waveform/metadata/ge=application/json;  # All return JSON.
    set cmd [list waveform metadata get $name]
    if {$key ne ""} {
        lappend cmd $key
    }
    set status [catch {eval $cmd} msg]
    if {$status} {
        return [SpecTcl::_returnObject "'waveform metadata get' failed" \
            [json::write string $msg]                                 \
        ]
    }
    set result [list]
    foreach metadata $msg {
        lappend result [json::write object \
            name [json::write string [lindex $metadata 0]] \
            value [json::write string [lindex $metadata 1]] \
        ]
    }
    SpecTcl::_returnObject OK [json::write array {*}$result]

}
## SpecTcl_waveform/metadata/set
#   Set a metadata item.
# Query params:
#   @param name - name of the waveform
#   @param key  - key of the metadata item to set.   This can appear several times.
#   @param value- value of the metadata item to set.  This can appear several times.
#           but must appear the same number of times as key.
#  Result:
#     On ok, the result is an empty detail.  Note that the name is validated
#     before the metadata are set as are the match in the number of metadata key/values.
#     This impllies the that this request either fully succeeds or fully fails.
#
proc SpecTcl_waveformm/metadata/set {name keys values} {
    set ::SpecTcl_waveform/metadata/set application/json;  # All return JSON.

    if {[llength $keys] != [llength $values]} {
        return [SpecTcl::_returnObject "'key' and 'value' parameters must appear the same number of times"]
    }
    set def [waveform list $name]
    if {[llength $def] == 0} {
        return [SpecTcl::_returnObject "No such waveform '$name'"]
    }
    set cmd [list waveform metadata set $name]
    foreach key $keys  value $values {
        lappend cmd $key $value
    }
    set status [catch {eval $cmd} msg]
    if {$status} {
        return [SpecTcl::_returnObject "'waveform metadata set' failed" \
            [json::write string $msg]                                 \
        ]   
    }
    SpecTcl::_returnObject OK

}
##
# SpecTcl_waveform/resize
#   Resize a waveform.
# Query params:
#   @param name - name of the waveform
#   @param samples - new number of samples.
# Result:
#   On success, the detail is empty.
#
proc SpecTcl_waveform/resize {name samples} {
    set ::SpecTcl_waveform/resize application/json;  # All return JSON.
    set status [catch {
        waveform resize $name $samples
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'waveform resize' failed" \
            [json::write string $msg]                          \
        ]
    } 
    SpecTcl::_returnObject OK
}