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
package require dns


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
    variable displayedGates;  # Info on the gate ids assigned to the displayed gates
    array set displayedGates [list]
    
    # See comments on Xamine::_removeDisplayedGates
    
    variable gatesDeletePending
    array set gatesDeletePending [list]
        
    
    
    variable traceToken [list]
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
# Xamine::getMirrorMemory
#   This is the same as getLocalMemory, but we need to set up or access
#   a mirror.
#   What we do is get what we need to call the compiled
#   Tcl wrapper for getSpecTclMemory.  On success, we'll hunt down
#   the mirror that was created for us...we'll then return the same two element list
#   that Xamine::getLocalMemory produces...for the local mirror.
#
proc Xamine::getMirrorMemory {} {
    puts "get mirror memory"
    set spectclHost [Xamine::getHost]
    set restPort    [Xamine::getPort]
    set mirrorPort  [Xamine::getMirrorPort]
    set spectclUser [Xamine::getUser]
    
    set status [catch {
        puts "Starting mirroring."
        Xamine::startMirrorIfNeeded \
            $spectclHost $restPort $mirrorPort $spectclUser
    } msg]
    if {$status} {
        error "Failed to start/attach mirror: $msg"
    }
    #  Figure out the mirrored memory key from the list of mirrors
    # SpecTcl is maintaining.
    
    set myHost [exec hostname]
    if {[Xamine::isLocal $myHost]} {
        set myHost localhost ;      # Host in the mirror list.
    }
    set mirrors [$Xamine::restClient mirror]

    puts "getMirrorMemory ($myHost : $spectclHost): \n $mirrors"

    foreach mirror $mirrors {
        set mirrorHost [dict get $mirror host]
        if {$myHost eq $mirrorHost} {
            return [list [dict get $mirror shmkey]  [$Xamine::restClient shmemsize]]
        }
    }
    # Rustogramer does not do a reverse dns so the mirror hosts are
    # IP addresses ... we might have several IPS as well:

    set dnstok  [dns::resolve $myHost]
    set myips [dns::address $dnstok]
    dns::cleanup $dnstok

    foreach mirror $mirrors {
	set mirrorHost [dict get $mirror host]
	foreach ip $myips {
	    if {$ip eq $mirrorHost} {
		return [list [dict get $mirror shmkey] [$Xamine::restClient shmemsize]]
	    }
	}
    }
    
    
    error "Mirror setup failed to set up a lasting mirror"    
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
    } elseif {$xGateType in [list "contour" "band"]} {
        Xamine:_Xamine2DGateToSpecTclGate $gatedef $spectrumDef
    } else {
        # Ignore all other types - silently.
    }
}
##
# Xamine::SpecTclGatesToXamineGates
#    This proc takes a collection of gates and displays the displayable
#    ones on the spectra that are bound into Xamine.  Note that this is naively an
#    O(m*n) (m = number of bound spectra and n the number of gates) problem.
#    and that, sadly, there can be a large number of spectra.
#    -  Remove all gates from the list that are never displayable, the only
#       displayable gates, after all are s, gs, b, c, gb, gc gates.
#    -  We then toss the gates into an array indexed by parameters in the gate
#      (this means that there will be more than one entry per gate, one for each)
#      parameter in the gate.  This is an attempt to remove the *n from the O(m*n).
#    - We then get a list of all spectrum definitions and all bindings.  The defs
#      are put in an array indexed by spectrum name
#    - We iterate over the bindings.  For each binding we:
#      *  use the spectrum's first parameter to get the candiate gates that can
#         be displayed on the spectrum,
#      *  Figure out if each gate is displayable on that spectrum.
#      *  Display that gate.
#
# @param gateDefs   - List of gate definitions from the REST interface.
# @note  Gates are only entered.  Normally to handle gate deletion/modification,
#        traces are established.  There are common procs between these two
#        procedures.
# 
proc Xamine::SpecTclGatesToXamineGates {gateDefs} {
    array set gatesByParameters [Xamine::_filterToDisplayableGates $gateDefs]
    
    
    # Now make an array of spectrum defs indexed by name for the bindings->def lookup.
    
    set spectra [$Xamine::restClient spectrumList]
    array set spectraByName [list]
    foreach def $spectra {
        set spectraByName([dict get $def name]) $def
    }
    # Now get and iterate over the bindings:
    
    set bindings [$Xamine::restClient sbindList]
    foreach binddef $bindings {
        #  For a bound spectrum get the gates that have at least the first
        #  parameter in common then iterate over those to figure out which
        # are displayable and display them.
        
        set def $spectraByName([dict get $binddef name])
        set firstpar [lindex [dict get $def parameters] 0]
        
        if {[array names gatesByParameters $firstpar] ne ""} {
            set candidateGates $gatesByParameters($firstpar)
            
            foreach g $candidateGates {
                if {[Xamine::_displayableGate $g $def]} {
                    
                    Xamine::_addGate $g $def $binddef
                }
            }
        }
    }
}
##
# Xamine::connectGates
#    Call this once at the beginning of your script execution:
#    *   All gates are listed and entered, if appropriate, to Xamine.
#    *   Gate traces are extablished and the gate trace handler
#        is used to  modyf the Xamine thread.
#    *   An Xamine gate handler is established and used to update SpecTcl's
#        understanding of gates.
# When this is all done:
#   * Xamine will have apprpriate initial gates displayed.
#   * New Xamine gates will automatically be reflected into SpecTcl
#   * Changes to SpecTcl gates will automatically be reflected in the set of
#     gates displayed by Xamine.
#
proc Xamine::connectGates { } {
    
    # Populate the initial gates in Xamine
    
    set initialGates [$Xamine::restClient gateList]
    Xamine::SpecTclGatesToXamineGates $initialGates
    
    # Establish gate traces to maintain Xamine Gates:
    
    set Xamine::traceToken [$Xamine::restClient traceEstablish 10]
    Xamine::_processSpecTclGateTraces
    
    #  Establish an event handler for new Xamine gates:
    
    Xamine::gate startPolling
    Xamine::gate setHandler Xamine::_processXamineGates
        
    
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
# Xamine::_parToChannel
#   Convert a parameter value to a spectrum channel.
#
# @param v - parameter value.
# @param axis - axis dict describing the transform.
# @return integer - channel number corresponding to v.
#
proc Xamine::_parToChannel {v axis} {
    set low [dict get $axis low]
    set high [dict get $axis high]
    set nch [dict get $axis bins]
    
    set result [expr {int(($v - $low)*$nch/($high - $low) + 0.5)}]
    
    return $result
}
##
# Xamine::_ptsToChannels2
#   Convert a list of points to channel coordinates.
#
# @param pts -list of  x y sublists.
# @param spec spectrum with axes defining the scaling
# @return list - list of  points converted to channel value.
#
proc Xamine::_ptsToChannels2 {pts spec} {
    set axes [dict get $spec axes]
    set xaxis [lindex $axes 0]
    set yaxis [lindex $axes 1]
    
    set result [list]
    foreach pt $pts {
        set x [Xamine::_parToChannel [lindex $pt 0] $xaxis]
        set y [Xamine::_parToChannel [lindex $pt 1] $yaxis]
        lappend result [list $x $y]
    }
    
    return $result
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
# Xamine::_channelToParam2
#    Convert a 2d point from spectrum channel coords to axis coords.
#
# @param xypoint
# @param xaxis
# @param yaxis
# @return xypoint
#
proc Xamine::_channelToParam2 {xypoint xaxis yaxis} {
    set x [lindex $xypoint 0]
    set y [lindex $xypoint 1]
    
    set xconv [Xamine::_channelToParam1 [list $x 0] $xaxis]
    set yconv [Xamine::_channelToParam1 [list $y 0] $yaxis]
    
    return [list $xconv $yconv]
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
##
# Xamine:_Xamine2DGateToSpecTclGate
#   Turn a 2d gate into a SpecTcl gate:
#   -   If Spectrum type is 2 band->b, contour -> c
#   -   If Spectrum type is g2 or gd  we're going to make a gb or gc
#   -   If Spectrum type is m2 - we need to make a gate for each parameter pair
#       and an OR gate to finish things off.
# @param gDef   - Gate definition dict.
# @param sDef   - Spectrum Definition dict.
#
proc Xamine:_Xamine2DGateToSpecTclGate {gDef sDef} {
    #  The base type is b or c..to which we may prepend g if this is a gamma
    #  gate.
    set status [catch {
    
    set gtype [dict get $gDef type]
    if {$gtype eq "contour"} {
        set gatetype c
    } else {
        set gatetype b
    }
    
    #  Everything else depends now on the spectrum type - note there are some
    #  spectrum types we need to ignroe
    
    set stype [dict get $sDef type]
    if {$stype in [list 2 g2 m2 gd]} {
        if {$stype in [list g2 gd]} {
            set gatetype g$gatetype
        }
        #  Convert the points.
        
    
        set xpoints [list]
        set ypoints [list]
        set xaxis [lindex [dict get $sDef axes] 0]
        set yaxis [lindex [dict get $sDef axes] 1]
        foreach pt [dict get $gDef points] {
            set point [Xamine::_channelToParam2 $pt $xaxis $yaxis]
            lappend xpoints [lindex $point 0]
            lappend ypoints [lindex $point 1]
        }
        # The parameters depend on spectrum type:
        
        if {$stype in [list 2 gd]} {
            set xparameters [lindex [dict get $sDef parameters] 0]
            set yparameters [lindex [dict get $sDef parameters] 1]
            
            $Xamine::restClient gateCreateSimple2D \
                [dict get $gDef name] $gatetype $xparameters $yparameters \
                $xpoints $ypoints
        } elseif {$stype eq  "g2" } {
            set xparameters [dict get $sDef parameters]
            set yparameters [list]
            $Xamine::restClient gateCreateSimple2D \
                [dict get $gDef name] $gatetype $xparameters $yparameters \
                $xpoints $ypoints
        } elseif {$stype eq "m2"} {
            #  For each parameter pair, we need a constituent gate of an
            # or gate the constituents are named
            #   _constituent_[dict get $gDef  name]_nn
            #
            set cnum 0
            set basename _constituent_[dict get $gDef name]_
            set constituents [list]
            foreach {xpar ypar} [dict get $sDef parameters] {
                set gateName $basename[incr cnum]
                $Xamine::restClient gateCreateSimple2D \
                    $gateName $gatetype $xpar $ypar $xpoints $ypoints
                lappend constituents $gateName
            }
            $Xamine::restClient gateCreateCompound [dict get $gDef name] + $constituents
        }
        
    }
    } msg]
    if {$status} {
        puts stderr "$msg : $::errorInfo"
    }
    #  Just ignore combinations that are not possible.
}
##
# Xamine::_displayableGate
#     Given the definition of a gate that has at least one parameter in common
#     with a spectrum and its definition determines if the gate is displayable on
#     the specified spectrum.  This is a complex question to answer that depends on
#     the types of both gate and spectrum and the parameters in both.
#
# @param gate    - Dict definition of the gate.
# @param spec    - Dict definition of spectrum.
# @return bool   - Boolean that is true if the gate can be displayed on the spectrum.
#                  false otherwise.
#
proc Xamine::_displayableGate {gate spec} {
    # The types:
    
    set gateType [dict get $gate type]
    set spectrumType [dict get $spec type]

    # In pretty much all cases once types are resolved it comes down to the
    # parameters:
    
    set gateParams [dict get $gate parameters]
    set spectrumParams [dict get $spec parameters]
    
    #  slices can only be displayed on 1-d spectra with the same parameter:
    
    if {($gateType eq "s") && ($spectrumType eq "1")} {
        if {$gateParams eq $spectrumParams} {
            return 1
        } else {
            return 0
        }
    }
    # Gamma slices can be displayed on a 1d spectrum with a common parameter or
    # a g1 spectrum with at least the parameters in the gate.
    
    if {$gateType eq "gs"} {
        if {$spectrumType eq "1"} {
            if {$spectrumParams in $gateParams} {
                return 1
            } else {
                return 0
            }
        } elseif {$spectrumType eq "g1"} {
            foreach param $gateParams {
                if {$param ni $spectrumParams} {
                    return 0
                }
            }
            return 1;     # Spectrum has all gate parameters.
        } else {
            return 0
        }
    }
    
    # Bands and contours can display on a 2d spectrum where there's a parameter
    # match (even if flipped).
    
    if {($gateType in [list b c]) && ($spectrumType eq "2")} {
        foreach param $gateParams {
            if {$param ni $spectrumParams} {
                return 0
            }
            return 1;             # All parameters are present.
        }
    }
    # Bands and contours can display on an m2 spectrum but the gate parameters must
    # 1. Be in the set of parameters in the spectrum.
    # 2. One of them must be on an x axis and the other on a y axis.
    # We use the trick that if n is even and m is odd then n + m must be odd to
    # determine the latter case, as the parameters come as x y x y in the list.
    # Note that in the pathalogical case that an m2 spectrum uses the same parameter
    # more than once we punt and say it's not displayable.
    #
    if {($gateType in [list b c]) && ($spectrumType eq "m2")} {
        set indices [list]
        set index 0
        foreach param $gateParams {
            if {$param in $spectrumParams} {
                lappend indices $index
            }
            incr $index
        }
        if {[llength $indices] != 2} {
            return 0;                 # not displayable
        }
        set indexSum [expr ([lindex $indices 0] + [lindex $indices 1]) % 2]
        return $indexSum;    # displayable if the sum is odd.
    }
    #  c, b are displayable on a gd spectrum if the x parameter of the gate is
    #  in the spectrum's x parameter list and the y parameter of the gate is in
    # the spectrum's y parameter (Or vica-versa).

    if {($gateType in [list c b]) && ($spectrumType eq "gd")} {
        # get spectrum x/y parameters by splitting the list at the midpoint.
        
        set nparms [llength $spectrumParams]
        set naxis [expr {$nparms/2}]
        
        set xparams [lrange $spectrumParams 0 $naxis-1]
        set yparams [lrange $spectrumParams $naxis end]
        
        if {([lindex $gateParams 0] in $xparams) && ([lindex $gateParams 1] in $yparams) } {
            return 1
        } elseif {([lindex $gateParams 1] in $xparams) && ([lindex $gateParams 0] in $yparams) } {
            return 1
        }
        return 0
    }
    
    # gb or gc are displayable on a 2d spectrum if the parameters in the spectrum
    # are both in the gamma gate parameters.
    #
    if {($gateType in [list gs gc]) && ($spectrumType eq "2")} {
        foreach param $spectrumParams {
            if {$param ni $gateParams} {
                return 0
            }
            return 1
        }
    }
    # gb and gc are displayable on gd only if the two parameter lists are
    # identical in content and order
    # (pretty much assuring that the gate was accepted on that
    # spectrum).
    
    if {($gateType in [list gb gc]) && ($spectrumType eq "gd")} {
        return [expr {$gateParams eq $spectrumParams}]
    }
    
    # No displayable conditions have been met:
    
    return 0    
}
##
# Xamine::fligpIfNeeded
#    If a gate and spectrum require that the x/y coordinates for gate points
#    be flipped this  proc does that
#
# @param gate - the gate.
# @param spec - The spectrum.
# @return list - list of points flipped or not as required.
#
proc Xamine::_flipIfNeeded {gate spec} {
    
    set points [dict get $gate points]
    set pointList [list]
    foreach point $points {
        lappend pointList [list [dict get $point x] [dict get $point y]]
    }
    set points $pointList
    
    set flip 0
    
    #  Figure out if we need to flip.   
    #
    set gateType [dict get $gate type]
    set gateParams [dict get $gate parameters]
    set specType [dict get $spec type]
    set specParams [dict get $spec parameters]
    if {($gateType in [list b c]) && ($specType eq "2")} {
        if {[lindex $gateParams 0] ne [lindex $specParams 0]} {
            set flip 1
        }
    }
    if {($gateType in [list b c]) && ($specType eq "m2")} {
        #  First half are X:
        set nParams [llength $specParams]
        set halfpt [expr {$nParams/2}]
        set xParams [lrange $specParams 0 $halfpt-1]
        if {[index $gateParams 0] ni $xParams} {
            set flip 1
        }
    }
    
    if {($gateType in [list b c]) && ($specType eq "gd")} {
        # X parameter must be in the even set.
        
        set xparameter [lindex $gateParams 0]
        foreach {x y} $specParams {
            if {$xparameter eq $y} {
                set flip 1
                break
            }
        }
    }
    
    #  If needed flip x/y on the points.
    
    if {$flip} {
        
        set flipped [list]
        foreach point $points {
            lappend flipped [list [lindex $point 1] [lindex $point 0]]
        }
    
        set points $flipped
    }
    
    return $points
}
##
# Xamine::_addGate
#   Adds a gate to Xamine
#
# @param gate - gate definition dict.
# @param spec - Spectrum definition dict.
# @apram binding - Binding dict.
# @note it's the callers responsibility to have determined the gate is, in fact,
#       displayable on the spectrum via e.g. Xamine::_displayableGate.
# @note It is possible, for 2d gates, that the points will need to have x/y
#       coordinates flipped in order to properly display. Consider, e.g. a
#       c gate with parameters p1, p2 displayed on a 2d spectrum on parameters
#       p2, p1.
# @note - a side effect of this is to record the gate in the list of displayed gates
#      (Xamine::displayedGates).   This is an array indexed by gate name containing
#      a list describing where the gate was displayed.  Each element of the
#      list is an Xamine spectrum id and the gate id assigned to the gate when entered in
#      that spectrum.
#
proc Xamine::_addGate {gate spec binding} {
    set xid [expr {[dict get $binding binding] + 1}];  # Xamine ids are 1+ binding.
    set gateType [dict get $gate type]
    set specType [dict get $spec type]

    #  Slice gates are straightforward.  We do need to map the values to channel
    #  coords.
    #
    
    if {$gateType in [list s gs]} {
        set axis [lindex [dict get $spec axes] 0]
        
        set low [Xamine::_parToChannel [dict get $gate low] $axis]
        set high [Xamine::_parToChannel [dict get $gate high] $axis]
        
        incr Xamine::gateId
    
        Xamine::gate add   \
            $xid $Xamine::gateId cut  \
            [dict get $gate name] [list [list $low 0] [list $high 0]]
        lappend Xamine::displayedGates([dict get $gate name]) [list $xid $Xamine::gateId cut]
        
        return
    }
    
    #  Band/contour  gates on 2d spectra are easy as well, once we work out
    #  the need or non-need to flip the coords of the gate
    
    if {($gateType in [list b c gs gc]) && ($specType in [list 2 g2 gd m2])} {
        set points [Xamine::_flipIfNeeded $gate $spec]
        set points [Xamine::_ptsToChannels2 $points $spec]
        
        if {$gateType in [list b gb]} {
            set xgtype band
        } else {
            set xgtype contour
        }
        incr Xamine::gateId
        
        Xamine::gate add \
            $xid $Xamine::gateId $xgtype   \
                [dict get $gate name] $points
        lappend Xamine::displayedGates([dict get $gate name]) [list $xid $Xamine::gateId $xgtype]
            
    }

}

##
# Xamine::_addNewGate
#    Given a new SpecTcl gate, gets is information and adds it to Xamine:
#
# @param name - name of new gate.
#
proc Xamine::_addNewgate {name} {
    set defs [$Xamine::restClient gateList $name]
    if {[llength $defs] > 0} {
       Xamine::SpecTclGatesToXamineGates $defs
    }
}
##
# Xamine::_deleteExistingGate
#    Delete an existing gate from Xamine
#
# @param name -name of the gate.
# @note the item describing this gate in Xamine::displayedGates is removed.
#
proc Xamine::_deleteExistingGate {name} {
    if {[array names Xamine::displayedGates $name] eq $name} {
        set xamineGateDefs $Xamine::displayedGates($name)
        
        # xamineGateDefs is a list because a gate can be displayed on more than
        # one Xamine Spectrum .
        
        foreach xamineGateDef $xamineGateDefs {
            
            set spectrumId [lindex $xamineGateDef 0]
            set gateId     [lindex $xamineGateDef 1]
            set type       [lindex $xamineGateDef 2]
            
            
            
            Xamine::gate remove $spectrumId $gateId $type
        }    
        array unset Xamine::displayedGates $name
    }
}
##
# Xamine::_removeDisplayedGates
#   Remove the gates formerly displayed on the xamine ID spectrum
#   xid from the displayed gates list.
#   This is normally called when a spectrum is unbound from that slot.
#
# So this is a bit more complex than you might think - locate the gate and
# tell Xamine to get rid of it but -- by this time, the spectrum the gate
# belongs to has been unbound and the remove operation will fail because of that.
# What we therefore do is put the gates that are displayed on that spectrum
# into Xamine::gatesDeletePending array indexd by XID.  When the spectrum is
# rebound, we will delete the appropriate gates prior to installing new ones.
#
# @param xid - Xamine Spectrum ID.  This is the first element of every
#              Xamine::displayedGates item.
# @note we're helped by the fact that a gate will only display on a spectrum once.
#       so once we find an xid match and remove it, we can go to the next
#       Xamine::displayedGate element.
#
proc Xamine::_removeDisplayedGates {xid} {
    foreach gateName [array names ::Xamine::displayedGates] {
        set index 0
        foreach instance $::Xamine::displayedGates($gateName) {
            set displayedSpectrum [lindex $instance 0]

            if {$xid == $displayedSpectrum} {
                lappend Xamine::gatesDeletePending($xid) $instance
                set Xamine::displayedGates($gateName) [lreplace      \
                    $Xamine::displayedGates($gateName) $index $index \
                ]
                
            }
            incr index
        }
        if {[llength $::Xamine::displayedGates($gateName)] == 0} {
            array unset Xamine::displayedGates $gateName
        }
    }
    
}
##
# Xamine::_processNewBindings
#    Called to handle new Spectrum bindings. For each spectrum we need to see
#    If any gates are displayable and, if so, display them.
#
# @param newBindings - list of 2 element sublists that contain name/xid pairs.
#
proc Xamine::_processNewBindings {newBindings} {
    # Foreach new spectrum, check each gate and, if it can be displayed on it,
    # display it.  The logic of this is very similar to that of
    # Xamine::SpecTclGatesToXamineGates an the implementation has resulted in
    # quite a bit of refactoring:
    
    # Convert newBindings into a form that looks a bit like the
    # sbindList format...but the Spectrum id is never used so we plunk in 0.
    # This loop is also a good time to delete any gates pending deletion for
    # the new bindings
    
    
    set mungedBindings [list]

    
    foreach binding $newBindings {
        set xid [lindex $binding 1]
        if {[array names Xamine::gatesDeletePending $xid] == $xid} {
            foreach gate $Xamine::gatesDeletePending($xid) {
                set gid [lindex $gate 1]
                set type [lindex $gate 2]
                
                Xamine::gate remove $xid $gid $type
            }
            array unset Xamine::gatesDeletePending $xid
        }
        
        lappend mungedBindings [dict create \
            spectrumid 0 name [lindex $binding 0]   \
            binding [expr {[lindex $binding 1] -1}]  \
        ]
    }
    
    
    #  Get the gates, reduced to those that can be displayed on any spectrum.
    
    set gates [$Xamine::restClient gateList]
    array set gatesByParameter [Xamine::_filterToDisplayableGates $gates]
    
    
    #  Iterate over the bound spectra figouring out if we can display
    #  any gate in that spectrum and, if so, display it.
    #  The assumption here is that there are not going to be that many new
    #  bindings other than one massive whap.
    #
    foreach binding $mungedBindings {
        set spectrumDef [lindex \
           [$Xamine::restClient spectrumList [dict get $binding name]] 0 \
        ]
        
        set firstPar    [lindex [dict get $spectrumDef parameters] 0]
        if {[array names gatesByParameter $firstPar] ne ""} {
            set candidateGates $gatesByParameter($firstPar)
            foreach g  $candidateGates {
                if {[Xamine::_displayableGate $g $spectrumDef]} {
                    Xamine::_addGate $g $spectrumDef $binding
                }
            }
        }
        
    }
    
    
}

##
# Xamine::_filterToDisplayableGates
#
#   Given a set of gates returns a list usable in array set to create a
#   gates list indexed by first parameter of only  the gates that can be
#   displayed somewhere.
#
# @param gateDefs
# @return list of two element pairs where the first element is the first
#         parameter of the gate and the second the list of gates definitions
#         that require/rely on that parameter.
#
#
proc Xamine::_filterToDisplayableGates {gateDefs} {
    array set result [list]
    foreach gate $gateDefs {
        if {[dict get $gate type] in [list s gs b gb c gc]} {
            foreach parameter [dict get $gate parameters] {
                lappend result($parameter) $gate
            }
        }
    }
    return [array get result]
}

##
# 
#----------------------------------------------------------------------------
#  Event handlers:
#

##
# Xamine::_processSpecTclGateTraces
#   Called periodically to fetch and process SpecTcl traces.  In this
#   implementaiton we only care about gate traces, hence the name.
#
proc Xamine::_processSpecTclGateTraces { } {
    set traces [$::Xamine::restClient traceFetch $Xamine::traceToken]
    
    # Changes in the gate dictionary 
    
    foreach gate [dict get $traces gate] {

        set type [lindex $gate 0]
        set name [lindex $gate 1]
        
        #  Adding a new gate:
        
        if {$type eq "add"} {
            Xamine::_addNewgate $name
        }
        
        #  Deleting an existing gate,
        
        if {$type eq "delete"} {
            Xamine::_deleteExistingGate $name
        }
        
        # Changing an existing gate.
        
        if {$type eq "changed"} {
            Xamine::_deleteExistingGate $name
            Xamine::_addNewgate $name
        }
    }
    #  Less obvious - changes in the set of spectra that are bound to the
    #  shared memory must also be monitored.  Specifically;
    #  - Unbinding requires any gates in the displayed  gates that was on that
    #    binding to be removed.
    #  - Binding requires figuring out and displaying any gates that
    #    need to be displayed on the newly bound spectrum.
    #
    set newBindings [list]
    
    foreach bindingChange [dict get $traces binding] {
        set what [lindex $bindingChange 0]
        set name [lindex $bindingChange 1]
        set xid  [lindex $bindingChange 2]
        incr xid;   # there's a +1 between spectcl and Xamine.
        
        if {$what eq "add"} {
            lappend newBindings [list $name $xid]
        } elseif {$what eq "remove"} {
            Xamine::_removeDisplayedGates $xid
        }
    }
    if {[llength $newBindings] > 0} {
        Xamine::_processNewBindings $newBindings
    }
    
    #  Reschedule
    
    after 1000 Xamine::_processSpecTclGateTraces
}
##
# Xamine::_processXamineGates
#    Called whenever an Xamine gate was queued to its IPC
#
# @param def - dictionary that describes the gate:
#     - spectrum - Xamine binding id of the spectrum on which the gate was set.
#     - id       - Graphical object id .
#     - type     - Type of object (e.g. cut).
#     - name     - Object name.
#     - points   - List of X/Y points in the gate.
#
proc Xamine::_processXamineGates {def} {

    Xamine::XamineGateToSpecTclGate $def
}
