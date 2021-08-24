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
    variable client ""
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
#------------------------------------------------------------------------------
#
# attach
#   Simulate the attach command in terms of the REST interface.
#
# @param args -parameter arguments.
# @return - what SpecTcl would have - for the most part.
#
#     
proc attach {args} {
    if {[llength $args] ==0} {
        error "'attach' command requires parameters"
    }
    set stype bad
    set size 8192
    set format ring
    
    for {set i 0} {$i < [llength $args]} {incr i} {
        set optname [lindex $args $i]
        if {$optname eq "-pipe"} {
            set stype pipe
        } elseif {$optname eq "-file"} {
            set stype file
        } elseif {$optname eq "-size"} {
            incr i
            set size [lindex $args $i]
        } elseif {$optname eq "-format"} {
            incr i
            set format [lindex $args $i]
        } elseif {$optname eq "-list"} {
            return [$SpecTclRestCommand::client attachList]
        } elseif {[string index $optname 0] eq "-"} {
            error "Unrecognized option $optname"
        } else {
            # This, and the rest are the data source:
            
            return [$SpecTclRestCommand::client attachSource \
                $stype [lrange $args $i end] $size $format         \
            ]       
        }
    }
    # If we get here there's nos ource spec:
    
    error "Missing data source specification."
}
#------------------------------------------------------------------------------
# sbind
#   Simulate the sbind command.  Forms:
#  sbind -all
#  sbind -list
#  sbind spectrum...
#
proc sbind {args} {
    if {[llength $args] == 0} {
        error "'sbind' command requires parameters"
    }
    set opt [lindex $args 0]
    if {$opt eq "-all"} {
        return [$SpecTclRestCommand::client sbindAll]
    } elseif {$opt eq "-list"} {
        set pattern *
        if {[llength $args] > 1} {
            set pattern [lindex $args 1]
        }
        set rawInfo [$SpecTclRestCommand::client sbindList $pattern]
        set  result [list]
        foreach sb $rawInfo {
            lappend result [list                                        \
                [dict get $sb spectrumid] [dict get $sb name] [dict get $sb binding] \
            ]
        }
        return $result
    } else {
        return [$SpecTclRestCommand::client sbindSpectra $args]
    }
}
#----------------------------------------------------------------------------
# Fit is a command ensemble and, therefore implemented as one:
#

namespace eval fit {
    namespace ensemble create
    namespace export create update delete list proc
    
    ##
    # fit::create
    #   Create a new fit.
    # @param name - name of the fit.
    # @param spectrum - spectrum name.
    # @param low, high - fit area of interest limits.
    # @param ftype - fit type.
    #
    proc create {name spectrum low high ftype} {
        return [$::SpecTclRestCommand::client fitCreate $name $spectrum $low $high $ftype]
    }
    ##
    # fit::update
    #
    #   Update some fits.
    # @param pattern - patter of fits to update.
    #
    proc update {{pattern *}} {
        return [$::SpecTclRestCommand::client fitUpdate $pattern]
    }
    ##
    # fit::delete
    #    Delete a fit.
    #
    # @param name name of the fit to delete.
    #
    proc delete {name} {
        return [$::SpecTclRestCommand::client fitDelete $name]
    }
    ##
    # fit::list
    #  List the fit results.
    #
    # @param pattern - pattern of fit names to match
    # @return list as described in the SpecTcl command reference guide.
    #
    proc list {{pattern *}} {
         
        set raw [$::SpecTclRestCommand::client fitList $pattern]
        
        set result [::list]
        
        foreach fit $raw {
        
            set name [dict get $fit name]
            set spectrum [dict get $fit spectrum]
            set type    [dict get $fit type]
            set low     [dict get $fit low]
            set high    [dict get $fit high]
            set params [::list]
            dict for {key value} [dict get $fit parameters] {
                lappend params [::list $key $value]
            }
            lappend result [::list $name $spectrum $type [::list $low $high] $params]
        }
        
        return $result
    }
    ##
    # fit::proc
    #  @param fit name.
    # @return procedure definiton to compute the fit.
    #
    proc proc {name} {
        return [$::SpecTclRestCommand::client fitProc $name]
    }
}
#-------------------------------------------------------------------------------
# Fold command - again a command ensemble.

namespace  eval fold {
    namespace export -apply -list -remove
    namespace ensemble create
    ##
    # -apply
    #   Apply a new fold.
    #
    # @param gate
    # @param args - the spectra to apply the fold to.\
    #
    proc -apply {gate args} {
        return [$::SpecTclRestCommand::client foldApply $gate $args]
    }
    ##
    # -list
    #    List the folds
    #
    # @param pattern - optional spectrum name match pattern.
    #
    proc -list {{pattern *}} {
        set rawInfo [$::SpecTclRestCommand::client foldList $pattern]
        set result [list]
        foreach fold $rawInfo {
            lappend result [list [dict get $fold spectrum ] [dict get $fold gate]]
        }
        return $result
    }
    ##
    # -remove
    #   Remove the fold from a spectrum.
    # @param spectrum
    #
    proc -remove {spectrum} {
        return [$::SpecTclRestCommand::client foldRemove $spectrum]
    }
}
