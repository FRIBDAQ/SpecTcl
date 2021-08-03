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
# @file channel.tcl
# @brief Provide a REST interface to the channel command.
# @author Ron Fox <fox@nscl.msu.edu>
#

package require json::write
Direct_Url /spectcl/channel SpecTcl_channel

##
# note that these are a bit tricky:
#   There must always be an x channel, there can be a y channel.
#   whether there _must_ be a y channel depends on the spectrum type.
#   we're going to cheat a bit.  Therefore we're always going to use the
#   args version of things and determine if we have one or two coordinates
#   based on the presence or absence of ychannel
#   We require spectrum, and in the case of set, the value.
#

##
# SpecTcl_channel/set
#    Set a channel in a spectrum.
# Query parameters:
# @param  spectrum - name of the spectrum.
# @param  xchannel - X channel to set
# @param  ychannel - Y channnel to set (optional use only if 2d).
# @param  value    - value to set
proc SpecTcl_channel/set {args} {
    set paramDict [SpecTcl::_marshallDict $args]
    
    set spectrum [dict get $paramDict spectrum]
    set xchannel [dict get $paramDict xchannel]
    set value    [dict get $paramDict value]
    
    if {[dict exists $paramDict ychannel]} {
        set ychannel [dict get $paramDict ychannel]
        set command [list channel -set $spectrum [list $xchannel $ychannel] $value]
    } else {
        set command [list channel -set $spectrum $xchannel $value]
    }
    set status [catch {
        {*}$command    
    } msg]
    
    if {$status} {
        SpecTcl::_returnObject "'channel -set' command failed: " [json::write string $msg]
    } else {
        SpecTcl::_returnObject OK
    }
    
}
##
# SpecTcl_channel/get
#    Return the contents of a channel
#    query parameters are:
#
# @param spectrum - spectrum name.
# @param xchannel - Required x channel number.
# @param ychannel - Y channel number only needed for 2d spectra
#
#  The detail is the value of the channel.
#
proc SpecTcl_channel/get {args} {
    set paramDict [SpecTcl::_marshallDict $args]
    set spectrum [dict get $paramDict spectrum]
    set xchannel [dict get $paramDict xchannel]
    
    if {[dict exists $paramDict ychannel]} {
        set ychannel [dict get $paramDict ychannel]
        set command [list channel -get $spectrum [list $xchannel $ychannel]]
    } else {
        set command [list channel -get $spectrum $xchannel]
    }
    set status [catch {
        {*}$command
    } v]
    
    if {$status} {
        SpecTcl::_returnObject "'channel -get' command failed: " [json::write string $v]
    } else {
        SpecTcl::_returnObject OK $v        
    }
}
    

    
