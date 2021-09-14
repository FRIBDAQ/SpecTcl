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
# @file   SpecTclUtils.tcl
# @brief  Package providing SpecTcl utilities in Tcl.
# @author Ron Fox <fox@nscl.msu.edu>
#

package provide SpecTclUtils 1.0
package require SpecTclRESTClient

#  Packages that will be available in the NSCLDAQ env:

if {[array names env DAQTCLLIBS] ne ""} {
    lappend auto_path $env(DAQTCLLIBS)
    package require portAllocator
}

namespace eval Xamine {
    variable restClient  [list];     # Rest client.
    variable haveDAQ
    if {[info command portAllocator] ne ""} {
        set haveDAQ 1
    } else {
        set haveDAQ 0
    }
    variable gateId 0  ;    # Xamine gate id number serial.
}

##
# Xamine::initRestClient
#   Initialize access to the REST client
#
# @param host - host in which the client lives.
# @param port - Port or service (service only if NSCLDAQ).
# @param user - User running SpecTcl (only needed for port as a service)
# @note On success, Xamine::restClient is set to the SpecTclRest client object
#       that will be used to communicate with SpecTcl.
#
proc Xamine::initRestClient {host port {user ""}} {
    
    # If the port is a service and we can look it up do so:
    
    if {![string is integer -strict $port]} {
        if {!$Xamine::haveDAQ} {
            error "Service lookup is only supported in the NSCLDAQ environment"
        }
        set pman [portAllocator %AUTO% -hostname $host]
        set svrport [$pman findServer $port $user]
        if {$svrport eq ""} {
            error "There's no service $port advertised by the user '$user'"
        }
        set port $svrport
    }
    # Now the port is an integer so we can create the rest interface:
    
    set Xamine::restClient [SpecTclRestClient %AUTO% -host $host -port $port]
    
    #  Figure out if this really works by doing a harmless operation:
    #  Note there's no assurance that SpecTcl won't later exit.
    
    set status [catch {
        $Xamine::restClient version
    } msg]
    if {$status} {
        $Xamine::restClient destroy
        error "Cannot fetch the SpecTcl version, make sure it is running $msg"
    }
}
##
# Xamine::getLocalMemory
#    Return information about the Xamine shared memory when SpecTcl is run
#    in the local host.  This can be used to setup the shared memory environment
#    variables needed by Xamine when we start it.
#
# @return two element list with key and size.
#
# @note initRestClient must have successfully run.
#
proc Xamine::getLocalMemory { } {
    set key [$Xamine::restClient shmemkey]
    set size [$Xamine::restClient shmemsize]

    return [list $key $size]
}

    
##
# Xamine::XamineGateToSpecTclGate
#   Given an Xamine gate as input, turns it into a SpecTcl gate which is
#   entered via the REST interface.
#   Here's a sketch of what must be done:
#   -  Figure out the SpecTcl spectrum on which the gate was accepted.
#   -  Based on the spectrum and gate types figure out the type of
#      SpecTcl Gate.
#   - Based on the spectrum axis definitions, translate the points into
#     parameter coordinates.
#   - Use the SpecTcl REST interface to enter the gate into Xmaine.
#
# @param  gateDef - Xamine gate definition dict.
# @note The Xamine spectrum id is 1 + the SpecTcl binding id.
#
proc Xamine::XamineGateToSpecTclGate {gatedef} {
    
    # Get the matching SpecTcl Spectrum definition dict:
    
    set bindingId [expr {[dict get $gatedef spectrum] - 1}]
    set spectrumDef [Xamine::_getSpectrumByBindingId $bindingId]
    
    # There are three gate types:  cut, contour and band. We'll handle each
    # in its own utility:
    
    set xGateType [dict get $gatedef type]
    if {$xGateType eq "cut"} {
        Xamine::_XamineCutToSpecTclGate $gatedef $spectrumDef
    } elseif {$xGateType eq "band"} {
        
    } elseif {$xGateType eq "contour"} {
        
    } else {
        error "Urecognized Xamine gate type: $xGateType"
    }
}

#------------------------------------------------------------------------------
# 'private' utilities.


##
# Xamine::_getSpectrumByBindingId
#    Given a binding id get the SpecTcl spectrum definition:
#
# @param id - binding id.
#
proc Xamine::_getSpectrumByBindingId {id} {
    set bindings [$Xamine::restClient sbindList]
    #
    #  Note by definition there will be a match:
    #
    foreach binding $bindings {
        if {$id eq [dict get $binding binding]} {
            break
        }
    }
    return [lindex [$Xamine::restClient spectrumList [dict get $binding name]] 0]
}
##
# Xamine::_channelToParam1
#  Convert a 1-d point in channel coordinates to axis coordinates.
#
# @param pt  - x/y point.
# @param axis - axis definition dict.
# @return double - Axis coordinate of X of pt.
#
proc Xamine::_channelToParam1  {pt axis} {
    set chan [lindex $pt 0]
    set alow [dict get $axis low]
    set ahi  [dict get $axis high]
    set chans [dict get $axis bins]
    
    return [expr {$chan*($ahi - $alow)/$chans + $alow}]
}
##
# Xamine::_XamineCutToSpecTclGate
#   If the spectrum was a 1 spectrum then this is a slice (s) gate
#   If the spectrum was a g1 spectrum this is a gamma slice (gs) gate.
#   Regardless, there's one axis and we use that to convert the points which
#   can then be used in gateCreateSimple1D.
#
# @param gateDef - Xamine gate definition.
# @param specDef - SpecTcl Spectrum on which the gate was accepted.
#
proc Xamine::_XamineCutToSpecTclGate {gateDef specDef} {
    
    #  Figure out gType - the SpecTcl gate type:
    
    set specType [dict get $specDef type]
    if {$specType eq "1"} {
        set gateType s
    } elseif {$specType eq "g1"} {
        set gateType gs
    } else {
        error "Unrecognized spectrum type: $specType"
    }
    # Convert the points to low/high
    
    set axes [dict get $specDef axes]
    set axis [lindex $axes 0]
    set gatePoints [dict get $gateDef points]
    
    set specGatePoints [list]
    foreach point $gatePoints {
        lappend specGatePoints [Xamine::_channelToParam1 $point $axis]
    }
    # Figure the slice olmits.
    
    set pts [join $specGatePoints ,]
    set low [expr min($pts)]
    set hi  [expr max($pts)]
    
    $Xamine::restClient gateCreateSimple1D \
        [dict get $gateDef name] $gateType [dict get $specDef parameters] $low $hi
    
}