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
# @file  SpecTclRestCommands.tcl
# @brief Simulate SpecTcl commands using the REST interface.
# @author Ron Fox <fox@nscl.msu.edu>
#




package provide SpecTclRestCommands 1.0
package require SpecTclRESTClient

##
#  The commands require a client object.  This has to be set with an
#  initialization operations:

namespace eval SpecTclRestCommand {
    variable client
}
#==============================================================================
# Private utilities.

##
# SpecTclRestClient::_gateDictToDef
#   Turn the dict for a gate that comes back from gateList into
#   a gate definition as SpecTcl native gate -list command might return.
# @param gate  - The gate dict describing the gate.
# @return list - gate definition list.
#
proc SpecTclRestCommand::_gateDictToDef {gate} {
    
    set name [dict get $gate name]
    set gtype [dict get $gate type]
    set result [list $name 0 $gtype];     # Gate id is meaningless..and not returned.
    if {$gtype in [list * + -]} {
        # Compound gates.
        lappend result [dict get $gate gates]
    } elseif {$gtype in [list F T]} {
        lappend result [list]
    } elseif {$gtype in [list gb gc]} {
        lappend result [dict get $gate points] [dict get $gate parameters]
    } elseif {$gtype in [list b c]} {
        lappend result [dict get $gate parameters] [dict get $gate points]
    } elseif {$gtype in [list s] } {
        lappend result \
            [dict get $gate parameters] \
            [list [dict get $gate low] [dict get $gate high]]
    } elseif {$gtype in [list gs]} {
        lappend result \
            [list [dict get $gate low] [dict get $gate high]] \
            [dict get $gate parameters]
    } elseif {$gtype in [list am em nm]} {
        lappend result [dict get $gate parameters] [dict get $gate value]
    } else {
        puts "Unrecognized gate type: $gtype"
    }
    
    return $result  
}
    


##
# SpecTclRestCommand::_getAppliedGateInfo
#   The REST apply interface only provides a gate name.  This
#   code turns that element into the gate definition the apply native
#   command returns.
#
# -  get all gate definitions
# -  Throw them up into an array indexed by gate name.
# -  For each reply from the REST server, convert it into what apply returns.
#
#
# @param info back from the pply command which is a list of dicts containing
#     spectrum - name of a spectrum.
#     gate     - name of the gate.
# @note the -TRUE- gate is special.
#
proc SpecTclRestCommand::_getAppliedGateInfo {info} {
    set gates [$::SpecTclRestCommand::client gateList]
    array set gateDefs [list]
    set gateDefs(-TRUE-) [list -TRUE- 0 T [list]];   # Special true gate.
    foreach gate $gates {
        set name [dict get $gate name]
        set gateDefs($name) [SpecTclRestCommand::_gateDictToDef $gate]
    }
    set result [list]
    foreach ap $info {
        set gateName [dict get $ap gate]
        set spectrumName [dict get $ap spectrum]
        lappend result [list $spectrumName $gateDefs($gateName)]
    }
    return $result
}
#==============================================================================
# Public entries.
##
# SpecTclRestCommand::initialize
#   Initialize the client object
#
# @param host - host the SpecTcl we control lives in
# @param port - Port the REST server is listening on.
#
proc SpecTclRestCommand::initialize {host port} {
    set SpecTclRestCommand::client [SpecTclRestClient \
        %AUTO% -host $host -port $port          \
    ]
}

#------------------------------------------------------------------------------
#
# apply
#    Simulate the apply command.  There are two versions:
#    -  apply gate spectrum....
#    -  apply -list ?pattern?
#  We are a bit more tolerant than the native command.
#  If there's junk after the pattern in apply -list we ignore it.
# @parm args - the command parameters.
# @note - yes we know this masks the apply command.
#
proc apply {args} {
    if {[llength $args] < 1} {
        error "'apply' command requires parameters"
    }
    set first [lindex $args 0]
    if {$first eq "-list"} {
        set pattern *
        if {[llength $args] > 1} {
            set pattern [lindex $args 1]
        }
        set info [$SpecTclRestCommand::client applyList $pattern]
        return [SpecTclRestCommand::_getAppliedGateInfo $info]
    } else {
        set gate $first
        set spectra [lrange $args 1 end]
        if {[llength $spectra] == 0} {
            error "'apply' needs at least one spectrum in addition to the gate."
        }
        return [$SpecTclRestCommand::client applyGate $gate $spectra]
    }
}


