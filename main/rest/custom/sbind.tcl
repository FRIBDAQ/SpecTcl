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
# @file  sbind.tcl 
# @brief Wrap REST server around the sbind command.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write

Direct_Url /spectcl/sbind  SpecTcl_sbind

##
# SpecTcl_sbind/all
#
# Simplest to do sbind -all as a separate subdomain:
#
#
proc SpecTcl_sbind/all {} {
    #
    #  This could fail if, e.g. the memory region is too small.
    #
    set status [catch {
        sbind -all
    } msg]
    
    if {$status} {
        SpecTcl::_returnObject "sbind -all failed:" [json::write string $msg]
    } else {
        SpecTcl::_returnObject "OK" 
    }
}

##
# SpecTcl_sbind/sbind
#    Bind a list of spectra to the display.
#
# @param  args - the key value pairs.
#
proc SpecTcl_sbind/sbind {args} {
    set spectra [dict get [::SpecTcl::_marshallDict $args] spectrum]
    
    set status [catch {
        sbind {*}$spectra
    } msg]
    
    if {$status} {
        SpecTcl::_returnObject "sbind -all failed:" [json::write string $msg]
    } else {
        SpecTcl::_returnObject "OK" 
    }
}
##
# SpecTcl_sbind/list
#    list spectrum bindings.
#    Can have a query parameters pattern that restricts the set of spectra.
#
#  The detail, on success, contains an array of objects that have:
#  - spectrumid - id of spectrum.
#  - name       - name of spectrum.
#  - binding    - binding id.
#
proc SpecTcl_sbind/list {{pattern *}} {

    set raw [sbind -list $pattern];   # I don't think this can fail.
    
    set bindingList [list]
    foreach binding $raw {
        lappend bindingList [json::write object                         \
          spectrumid [lindex $binding 0]                                \
           name      [json::write string [lindex $binding 1]]           \
           binding   [lindex $binding 2]                                \
        ]
    }
    
    SpecTcl::_returnObject OK [json::write array {*}$bindingList]
}