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
#             Jeromy Tompkins 
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321


##
# @file version.tcl
# @brief Provide interface to the version of SpecTcl.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write

Direct_Url /spectcl/version SpecTcl_version

##
# SpecTcl_version
#    Return the SpecTcl version.
#    - If possible, the version command is used to produce M.m-edit
#    - If not, $SpecTclHome/VERSION is parsed into M.m-edit
#    - From there the JSON Object with the following attributes is returned:
#      - major - major version (M above).
#      - minor - minor version (m above).
#      - editlevel - Edit level of the version (edit above).
#
proc SpecTcl_version {} {
    if {![catch version msg]} {
        set vsn $msg
    } else {
        set f [open [file join $::SpecTclHome VERSION] r]
        set contents [read $f]
        close $f
        
        set nameAndVersion [lindex $contents 1];                      # E.g SpecTcl-3.5-002
        set versionList    [lrange [split $nameAndVersion -] 1 end]; # 3.5 002
        set vsn [join $versionList -];                               # 3.5-002
    }
    #   Here vsn is of the form M.m-edit
    
    set l1 [split $vsn .];     # M m-edit
    set l2 [lreplace $l1 1 end [split [lindex $l1 1 end] -]];   # M {m edit.}
    set major [lindex $l2 0]
    set minor [lindex [lindex $l2 1] 0]
    set edit  [lindex [lindex $l2 1] 1]
    
    return [::SpecTcl::_returnObject OK                                   \
                [json::write object                                       \
                    major $major  minor $minor                            \
                    editlevel [json::write string $edit]                  \
                 ]                                                        \
           ]
}
