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
# @file fold.tcl
# @brief REST Server implementation for fold command
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/fold SpecTcl_fold

##
# SpecTcl_fold/apply
#    Apply a fold to one or more spectra
# Query parameters:
# @param gate  - name of the gate to apply as a fold.
# @param spectrum - one or more occurences of spectra.
#
proc SpecTcl_fold/apply {args} {
    set params [SpecTcl::_marshallDict $args]
    set gate [dict get $params gate]
    set spectra [dict get $params spectrum]
    
    set status [catch {
        fold -apply $gate {*}$spectra
    } msg]
    if {$status} {
        SpecTcl::_returnObject "'fold -apply' command failed" [json::write string $msg]
    } else {
        SpecTcl::_returnObject "OK"
    }
}
##
# SpecTcl_fold/list
#    List folds that match a glob pattern that defaults to *
#
# @param pattern - Pattern to match spectra against.  Defaults to *
#
# The detail contains an array of objects that have attributes:
#   -  spectrum - the name of a folded spectrum.
#   -  gate     - Name of the gate folded onthe spectrum.
#
proc SpecTcl_fold/list {{pattern *}} {
    set status [catch {
        fold -list $pattern
    } data]
    
    if {$status} {
        SpecTcl::_returnObject "'fold -list' failed: " [json::write string $data]
    } else {
        set objectList [list]
        foreach fold $data {
            lappend objectList [json::write object                      \
                spectrum [json::write string [lindex $fold 0]]          \
                gate     [json::write string [lindex $fold 1]]          \
            ]
        }
        SpecTcl::_returnObject OK [json::write array {*}$objectList]
    }
}
##
# SpecTcl_fold/remove
#   Remove any folds from a spectrum.
#
# @param spectrum - name of the spectrum to unfold.
#
proc SpecTcl_fold/remove {spectrum} {
    set status [catch {
        fold -remove  $spectrum
    } msg]
    if {$status} {
        SpecTcl::_returnObject "'fold -remove' command failed: " [json::write string $msg]
    } else {
        SpecTcl::_returnObject OK
    }
}