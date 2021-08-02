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
    SpecTcl::_returnObject OK [json::write string [attach -list]]
}

    

    
