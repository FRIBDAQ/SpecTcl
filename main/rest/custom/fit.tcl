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
# @file  fit.tcl
# @brief Provide REST server access to the SpecTcl fit command.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write

Direct_Url /spectcl/fit SpecTcl_fit

##
# _propListToDict
#    Convert a property list to an equivalent dict.
#
# @param props -  list of name /value pairs.
# @return dict with keys as names.
#
proc _propListToDict {props} {
    set result [dict create]
    foreach property $props {
        dict set result [lindex $property 0] [lindex $property 1]
    }
    return $result
}

##
# _fitToObject
#  Turn a fit result list into a JSON encoded object.
#
# @param fit - fit information.
# @return JSON Object.
#
proc _fitToObject {fit} {
    set fitName      [lindex $fit 0]
    set spectrumName [lindex $fit 1]
    set fitType      [lindex $fit 2]
    set low          [lindex [lindex $fit 3] 0]
    set hi           [lindex [lindex $fit 3] 1]
    set parameters [json::write object {*}[_propListToDict [lindex $fit 4]]]
    return [json::write object name [json::write string $fitName]   \
            spectrum [json::write string  spectrumName]              \
            type [json::write string  $fitType]                      \
            low $low high $hi parameters $parameters                 \
    ]
}

#-------------------------------------------------------------------------------
##
# SpecTcl_fit/create
#    Create a new fit.  We need
# @param name  - Name of the new fit.
# @param spectrum - name of the spectrum being fit.
# @param low    - Low spectrum channel limit for of the fit.
# @param high   - High channel limit for the fit.
# @param type   - Fit type e.g. 'gaussian'.
#
proc SpecTcl_fit/create {name spectrum low high type} {

    set status [catch {
        fit create $name $spectrum $low $high $type
    } msg]
    
    if {$status} {
        SpecTcl::_returnObject "'fit'command failed: " [json::write string $msg]
    } else {
        SpecTcl::_returnObject OK
    }
}
  
##
# SpecTcl_fit/update
#   Recompute fit values in fits that match a pattern.
# @param pattern - optional glob pattern "*" is the default which matches all.
#
proc SpecTcl_fit/update {{pattern *}} {
    set status [catch {
        fit update $pattern
    } msg]
    if {$status} {
        SpecTcl::_returnObject "'fit'command failed: " [json::write string $msg]
    } else {
        SpecTcl::_returnObject OK
    }
}

##
# SpecTcl_Fit/delete
#   Delete a single fit object:
# @param name
#
proc SpecTcl_fit/delete {name} {
    set status [catch {
        fit delete $name
    } msg]
    
    if {$status} {
        SpecTcl::_returnObject "'fit'command failed: " [json::write string $msg]
    } else {
        SpecTcl::_returnObject OK
    }
}
##
# SpecTcl_fit/list
#   Lists matching fits.  The details will be an array with the
#   following attributes:
#   - name - fit name.
#   - spectrum - name of spectrum being fit.
#   - type  - Fit type.
#   - low   - low channel of the fit area of interest.
#   - high  - high channel of the fit area of interest.
#   - parameters - object with attributes and values that
#              specify the fit results. Attribute names are fit parameter
#              names and attribute values are their values.
#
# @param pattern - glob pattern whose matches are the fits that will be listed.
#
proc SpecTcl_fit/list {{pattern *}} {

    set status [catch {
        fit list $pattern
    } msg]
    if {$status} {
        SpecTcl::_returnObject "'fit'command failed: " [json::write string $msg]
    } else {
        set resultList [list]
        foreach result $msg {
            lappend resultList [_fitToObject $result]
        }
        SpecTcl::_returnObject "OK" [json::write array {*}$resultList]
    }
    
}

    


    


