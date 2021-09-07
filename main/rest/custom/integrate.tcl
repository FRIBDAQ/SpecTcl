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
# @file   integrate.tcl
# @brief Provide access to the SpecTcl integrate command
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write

Direct_Url /spectcl/integrate SpecTcl_integrate

##
# SpecTcl_integrate.
#    Access the integrate command. There are three types of integrations
#    we can perform:
#   - Integration inside a gate. (query param gate).
#   - Integration in a 1d spectrum (query params low high)
#   - Integration in a 2d spectrum (query params xcoords ycoords defining a
#      polygon (at least 3 points)).
#
# @param args  - holds the query parameter dict.  Keys:
#         -  spectrum - name of the spectrum in which we're integrating.
#         -  gate     - name of the gate if integrating in a gate.
#         -  low,high - 1 d integration limits.
#         -  xcoord, ycoord - X/Y coordinates of the polygon in which to do a 2d
#            integration.
# @return JSON returned detail contains an object with the keys:
#     -  centroid - which can be an array of two coordinates if 2-d
#     -  fwhm     - which can be an array of two coordinates if 2-d
#     -  counts   - Total counts in the ROI.
#
# @note If the request is ambiguous (e.g. gate and points), the order in which
#       We resolve the integration is gate, followed by low, high, followed by 
#      x,y coordinates.
#
#
proc SpecTcl_integrate {args} {
    set ::SpecTcl_integrate application/json
    set qdict [::SpecTcl::_marshallDict $args]
    
    if {![dict exists $qdict spectrum]} {
        return [SpecTcl::_returnObject "Missing required query parameter 'spectrum'"]
    }
    set spectrum [dict get $qdict spectrum]
    
    #  Is this an integration in a gate:
    
    if {[dict exists $qdict gate]} {
        set command [list integrate $spectrum [dict get $qdict gate]]
    } elseif {[dict exists $qdict low]} {
        # 1d need a high:
        
        if {![dict exists $qdict high]} {
            return [SpecTcl::_returnObject "Missing required query parameter 'high'"]
        }
        set command [list integrate $spectrum \
            [list [dict get $qdict low] [dict get $qdict high]]
        ]
    } elseif {[dict exists $qdict xcoord]} {
        if {![dict exists $qdict ycoord]} {
            return [SpecTcl::_returnObject "Missing required query parameter 'ycoord'" ]
        }
        set pointList [::SpecTcl::_marshallXYPoints $qdict]
        
        if {[llength $pointList] < 3} {
            return [SpecTcl::_returnObject "There must be at least 3 'xcoord' and 'ycoord' parameters"]
        }
        set command [list integrate $spectrum $pointList]
    } else {
        return [SpecTcl::_returnObject "'integrate' - could not determine integration type"]
    }
    #  If we got here, command has the integration command to perform.
    #  we run it and figure out what we've got.
    #
    
    set status [catch {
        {*}$command
    } info]
    if {$status} {
        return [SpecTcl::_returnObject "'$command' failed"      \
            [json::write string $info]                          \
        ] 
    
    }
    set centroid [lindex $info 0]
    set fwhm     [lindex $info 2]
    set sum      [lindex $info 1]
    
    #  If the centroid and fwhm are arrays we need to encode them as such.
    
    if {[llength $centroid] > 1} {
        set centroid [json::write array {*}$centroid]
        set fwhm     [json::write array {*}$fwhm]
    }
    #  Now we can return the result:
    
    return [SpecTcl::_returnObject OK [json::write object   \
        centroid $centroid                                  \
        fwhm     $fwhm                                      \
        counts   $sum                                       \
    ]]
    
}