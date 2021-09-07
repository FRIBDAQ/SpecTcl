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
# @file  sread.tcl
# @brief Use the sread command to read a spectrum from file.
# @author Ron Fox <fox@nscl.msu.edu>
#
package require json::write
Direct_Url /spectcl/sread SpecTcl_sread

##
#  Comput a flag value
# @param flagname flag name
# @param value  name of the value
# @return string $flagname if $value else -$flagname
proc _flagValue {flagname value} {
    if {$value} {
        return -$flagname
    } else {
        return -no$flagname
    }
}

##
# Note that there are some limitiations in that we cannot support
# reading from a fd opened within SpecTcl, only from file.
#

##
# SpecTcl_sread
#
# @param filename   -  name of the file.
# @param format     - file format (defaults to ascii)
# @param snapshot   - FLag determining if the resulting spectrum is a snapshot
#                     by default it is.
# @param replace     - Flag determining if an existing spectrum with the same
#                     name is replaced. The default is that it is not.
# @param bind       - flag that determines if the resulting spectrum  is bound
#                     to the displayer memory (by default it is).
proc SpecTcl_sread {filename {format ascii} {snapshot 1} {replace 0} {bind 1}}  {
    set status [catch {
            
        sread -format $format  \
            [_flagValue snapshot $snapshot] \
            [_flagValue replace $replace]   \
            [_flagValue bind $bind] $filename
    } msg]
    
    if {$status} {
        return [SpecTcl::_returnObject "'sread' command failed" \
            [json::write string $msg]
        ]
    }
    return [SpecTcl::_returnObject OK]
}
