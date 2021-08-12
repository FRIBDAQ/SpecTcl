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
# @file swrite.tcl
# @brief write spectra to file.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/swrite SpecTcl_swrite

##
# SpecTcl_swrite
# @param file - name of the file to write to.
# @param format - Format to use
# @param spectrum - one or more spectra to write.
#  File and spectrum are required. The format defaults to 'ascii'
#
proc SpecTcl_swrite {args} {
    set ::SpecTcl_swrite application/json
    
    set qdict [::SpecTcl::_marshallDict $args]
    if {![dict exists $qdict file]} {
        return [SpecTcl::_returnObject "Required 'file' query parameter missing"]
    }
    if {![dict exists $qdict spectrum]} {
        return [SpecTcl::_returnObject "Required 'spectrum' query parameter missing"]
    }
    set file [dict get $qdict file]
    set spectra [dict get $qdict spectrum]
    if {[dict exists $qdict format]} {
        set format [dict get $qdict format]
    } else {
        set format ascii;             # Default.
    }
    
    set status [catch {
        swrite -format $format $file {*}$spectra
    } msg]
    if {$status} {
        return [SpecTcl::_returnObject "'swrite' command failed" \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}
    
