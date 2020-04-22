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
# @file dbconfig.tcl
# @brief Support storing and retrieving configurations and spectra from sqlite3 database.
# @author Ron Fox <fox@nscl.msu.edu>
#
package provide dbconfig 1.0
package require SpecTclDB
package require sqlite3
namespace eval dbconfig {
#----------------------------------------------------------
#  Private procs.

##
# _lookupSaveSet
#
# @param cmd - database command.
# @param savename - name of a saveset.
# @return integer -save set id or
# @throw error if there's no matching saveid.
#
proc _lookupSaveSet {cmd savename} {
    set saveId ""
    
    $cmd eval {
        SELECT id FROM save_sets WHERE name = :savename
    } {
        set saveId $id
    }
    if {$saveId eq ""} {
        error "There is no save set named $savename"
    }
    return $saveId
}

##
# _saveParameters
#    Save the parameter definitions to the parameter_defs table:
#
# @param saveset - the saveset object instance command.

proc _saveParameters {saveset} {
    foreach rawDef [parameter -list *] {
        set name [lindex $rawDef 0]
        set number [lindex $rawDef 1]
        set tdef [treeparameter -list $name]
        if {[llength $tdef] != 0} {
            
            ## Full def:
            
            set tdef [lindex $tdef 0]
            set bins [lindex $tdef 1]
            set low  [lindex $tdef 2]
            set high [lindex $tdef 3]
            set units [lindex $tdef 5]
            
            $saveset createParameter $name $number $low $high $bins $units
            
        } else {
            # only save name and number:
            
            $saveset createParameter $name $number
            
        }
    }
}
##
# _saveSpectrumDefs
#    Saves the spectrum definitions.
#    It's not really necessary to know anything about what a spectrum is to do this
#    given its spectrum definitions (unlike e.g.gate defs)
# @note Spectrum definitions are going to involve insertions into three tables, however
#      spectrum_defs, axis_defs, and spectrum_params  The spectrum_params
#      insert requires a lookup into the parameter_defs  table.
#
# @param saveset - saveset instance command.
#
proc _saveSpectrumDefs {saveset} {
    foreach def [spectrum -list *] {
        set name [lindex $def 1]
        set type [lindex $def 2]
        set params [lindex $def 3]
        set axes   [lindex $def 4]
        set datatype [lindex $def 5]
        
        $saveset createSpectrum  $name $type $params $axes $datatype
    }
}


##
# _canWrite
#    Determines if a gate is writable. A gate is writable if
#   - it is primitve or
#   - it is not primitive but all gates it depends on have been written.
#
# @param gate - the definition of the gate to check.
# @param written - the names of the gates that have been written.
# @return bool - true if the gate is writable.
#
proc _canWrite {gate written} {
    set type [lindex $gate 1]
    if {$type in [list * + -]} {
        # Need dependency analysis.
        
        set dependencies [lindex $gate 2]
        foreach dep $dependencies {
            if {$dep ni $written} {
                return 0;          #unwritten dependency.
            }
        }
        #  All dependencies are in the written list:
        
        return 1
        
    } else {
        return 1;              # Can always write a primitive gate.
    }
}
##
# _ReorderGates
#    SpecTcl gates can have dependencies.  Compound gates like * + - types
#    depend on other gates.  Because the component_gates table just stores
#    gate ids and not gate names (it's a join table); it's imporant to
#    write gates out in reverse dependency order (that is all gates a gate is
#    dependent on must be written prior to that gate).
#    This code re-orders gate definitions to make that happen:
#    - We put all prmitive gates first - they have no depenencies.
#    - Then we keep writing out compound gates whose dependencies are all
#      written until none are left or until the number of gates remaining does
#      not decrease (that indicates a cyclical dependency that should not actually
#      be possible).
#
# @param defs   - list of gate definitions.
# @return list  - reordered gate definitions.
#
proc _ReorderGates {defs} {
    
    set result [list]
    set written [list];            # names of gatesin result.
    
    while {[llength $defs] > 0} {
       
        set idxs [list]
        set i 0;                  # Keeps track of indices.
        foreach gate $defs {
           if {[_canWrite $gate $written]} {
            
            lappend idxs $i
            lappend result $gate
            lappend written [lindex $gate 0]
           } 
           incr i
        }
        #  Did we write anything?
        
        if {[llength $idxs] == 0} {
            error "Gate dependency cycle detected!!!"
        }
        # Remove the written gates from defs in revers order so we don't have
        # to do any index readjustment:
        
        set idxs [lreverse $idxs]
        foreach idx $idxs {
            set defs [lreplace $defs $idx $idx]   ; # Remove a writte element.
        }
    }
    return $result
}

##
# _saveGateDefinitions
#    Save definitions of gates.  This one has tones of special cases depending
#    on the gate type.
#
# @param saveset - the saveset instance command
#
proc _saveGateDefinitions {saveset} {
    set gates [gate -list]
    
    # Strip the gate of its id (element1)
    
    set strippedGates [list]
    foreach gate $gates {
        lappend strippedGates [lreplace $gate 1 1]
    }
    set gates $strippedGates
    
    
    #
    #   The gates must be re-ordered so that compound gates are only written
    #        Once their component gates are written.
    
    set gates [_ReorderGates $gates]
    
    
    foreach gate $gates {
        set name [lindex $gate 0]
        set type [lindex $gate 1]
        set descr [lindex $gate 2]
    
        # How a gate is entered depends on its type.
        
        if {$type in [list s gs] } {
            # 1d gate.
            
            if {$type eq "s"} {
                set params [lindex $descr 0]
                set lims [lindex $descr 1]
                set low    [lindex $lims 0]
                set  high [lindex $lims 1]
                
            } else {
                set lims [lindex $descr 0]
                set params [lindex $descr 1]
                set low [lindex $lims 0]
                set high [lindex $lims 1]
            }
            $saveset create1dGate $name $type $params $low $high
        } elseif {$type in [list c gc b gb]} {
            
           
            if {$type in [list c b]} {
                set points [lrange $descr 1 end]
                set params [lindex $descr 0]
            } else {
                set points [lindex $descr 0]
                set params [lindex $descr 1]
            }
            
            $saveset create2dGate $name $type $params $points
        } elseif {$type in [list * + - T F]} {
            
            $saveset createCompoundGate $name $type $descr

        } elseif {$type in [list em am nm]} {
            set param [lindex $descr 0]
            set mask  [lindex $descr 1]
            
            $saveset createMaskGate $name $type $param $mask
        } else {
            error "Unknonw gate type; $type"
        }
    }
        
}
##
# _saveGateApplications
#    Saves the applications of gates to spectra.
#   At this point we assume that
#   -   All spectra have been defined.
#   -   All gates have been defined.
#
#
# @param saveset -the saveset instance command
#
proc _saveGateApplications {saveset} {
    set applications [apply -list]
    
    #  Now run through the applications. Note that if the gate is named -TRUE-
    # the spectrum is ungated and we won't list an application for it:
    
    foreach application $applications {
        set spname [lindex $application 0]
        set gname  [lindex [lindex $application 1] 0]
        if {$gname ne "-TRUE-"} {
            $saveset applyGate $gname $spname
        }
    }
    
}
##
# _saveTreeVariables
#    Saves tree variables/names.
# @param saveset - saveset instance command
#
proc _saveTreeVariables {saveset} {
    set defs [treevariable -list]
    foreach def $defs {
        set name [lindex $def 0]
        set value [lindex $def 1]
        set units [lindex $def 2]
        $saveset createVariable $name $value $units
        
    }
}

##
# _saveSpectrumChans
#   Saves the channels in one spectrum to the database.
#
# @param cmd - data base command.
# @param specid - spectrum id in the database.
# @param data - the data from scontents this is a list of the non-zero channels
#               each element is a two or three elmeent list depending on the
#               spectrum dimensionality.  1d spectrum data are
#               channnel value pairs.  2d spectrumdata are xchan ychan triplets.
# @note since data can be rather large, we determine the dimensionality from the
#       first element of the data:
#
proc _saveSpectrumChans {cmd specid data} {
    
    if {[llength $data] > 0} {
        if {[llength [lindex $data 0]] == 2} {
            # 1d
            foreach chan $data {
                set bin [lindex $chan 0]
                set value [lindex $chan 1]
                $cmd eval {
                    INSERT INTO spectrum_contents
                    (spectrum_id, xbin, value)
                    VALUES(:specid, :bin, :value)
                }
            }
            
        } else {
            # 2d
            foreach chan $data {
                set x [lindex $chan 0]
                set y [lindex $chan 1]
                set value [lindex $chan 2]
                $cmd eval {
                    INSERT INTO spectrum_contents
                    (spectrum_id, xbin, ybin, value)
                    VALUES(:specid, :x, :y, :value)
                }
            }
        }
    } else {
        #  In this case we need to put channel 0 place holders
        #  Otherwise the system will think we have not saved this spectrum.
        #  We need to figure out how many dimensions the spectrum has in order
        #  to do this properlyh:
        
        set type [$cmd eval {SELECT type FROM spectrum_defs WHERE id = :specid}]
        if {$type in [list 1 g1 b s] } {
            set ychan ""
        } else  {
            set ychan 0
        }
        $cmd eval {
            INSERT INTO spectrum_contents (spectrum_id, xbin, ybin, value)
                VALUES(:specid, 0, :ychan, 0)
        }
        
    }
}
##
# _saveSpectraContents.
#    Saves the channels in al spectra into the database:
#
# @param saveset -the saveset instance command.
# @note the assumption is that the spectrum definitions have already been saved.
#
proc _saveSpectraContents {saveset} {
    # Get a list of spectrum names in this save id:
    
    set specs [$saveset listSpctra]
    foreach spectrum $specs {
        set name [dict get $spectrum name]
        $saveset storeChannels $name [scontents $name]
    }
        
}
##
# _restoreTreeParam
#    If the tree parameter exists, update it otherwise make it.  While the
#    may not know about it, the definition is useful for creating spectra.
#
# @param name - parameter name.
# @param low  - Recommended low spectrum limit.
# @param high - Recommended high spectrum limit.
# @param bins - Recommended number of bins.
# @param units - Units of measure.
#
proc _restoreTreeParam {name low high bins units} {
    if {[llength [treeparameter -list $name]] > 0} {
        # Update.
        
        treeparameter -setunit   $name $units
        treeparameter -setlimits $name $low $high
        treeparameter -setbins   $name $bins
        
    } else {
        treeparameter -create $name $low $high $bins $units
    }
}
##
# _restoreParamDefs
#    Restores the parameter definitions. 
#    - If the parameter exists it is deleted and a new one created.  Note
#      That if the id is not modified by this any tree parameters bound will
#      retain their binding.
#    - If the parameter has range, binning or units information:
#      * If there's no existing tree paramter a new one is created.
#      * If there's an existing tree parameter it is modified.
#
# @param saveset - saveset instance command.
#
proc _restoreParamDefs {saveset} {
    set paramDefs [$saveset listParameters]
    
    foreach def $paramDefs {
        set name [dict get $def name]
        set number [dict get $def number]
        if {[llength [parameter -list $name]] > 0} {
            parameter -delete $name
        }
        parameter -new $name $number
        
        # Is it a tree param?
        
        if {[dict exists $def low]} {
            set low [dict get $def low]
            set high [dict get $def high]
            set bins [dict get $def bins]
            set units [dict get $def units]
            
            _restoreTreeParam $name $low $high $bins $units
        } 
    }
    
}
##
# _restoreSpectrumDefs
#    Restores the spectrum definitions for a saveset.  Any existing spectrum
#    with the same name is deleted first.
#
# @param saveset  - saveset instance command
# @param spname - if provided the name of the single specstrum id we want
#               to restore.
#
proc _restoreSpectrumDefs {saveset {spname {}}} {
    # Get the spectrum top level defs.  These will go into an array of
    # dicts indexed by the spectrum id
    
    if {$spname ne ""} {
        set speclist [list [$saveset findSpectrum $spname]]
    } else {
        set speclist [$saveset listSpectra]
    }
     
    # Speclist is now the list of spectra to restore.
    
    # Now we can run over the spectra, deleting existing ones and
    # Creating the ones in the definition.
    
    
    foreach def $speclist {
    

        set name [dict get $def name]
        if {[llength [spectrum -list $name]] != 0 } {
            spectrum -delete $name
        }
        # Construct the spectrum definition command:
        
        set cmd [list spectrum -new $name]
        lappend cmd [dict get $def type] [dict get $def parameters]
        set axisDefs [dict get $def axes]
        set axiscmd [list]
        foreach adef $axisDefs {
            set low [dict get $adef low]
            set high [dict get $adef high]
            set bins [dict get $adef bins]
            lappend axiscmd [list $low $high $bins]
        }
        lappend cmd $axiscmd [dict get $def datatype]

        eval $cmd
    }
}
##
# _is2d
#  Returns true if a spectrum type has two dimensions.
#
# @param name -spectrum name
# @return bool
#
proc _is2d {name} {
    set def [spectrum -list $name]
    set def [lindex $def 0]
    set type [lindex $def 2]  ; #id name type...
    
    set is1 [expr {$type in [list 1 g1 b S]}];  #Fewer 1ds.
    return [expr {!$is1}]
}
##
# _restoreChannel
#   Restore a spectrum channel:
#
# @param name - spectrum name
# @param xbin - x axis bin number.
# @param ybin - y axis bin number ("" if one dimensional).
# @param value - channel value.
#
proc _restoreChannel {name xbin ybin value} {
    
    set cmd [list channel -set $name]
    if {[_is2d $name]} {
        lappend cmd [list $xbin $ybin]
    } else {
        lappend cmd $xbin
    }
    lappend cmd $value
    
    eval $cmd    
}
##
# _restoreSpectrumContents
#    Restores the spectrum contents for a save set.
#    The assumption is that the spectrum has been defined.
#
# @param saveset - saveset instance command
# @param spname  - optional name of single spectrum to restore
#
proc _restoreSpectrumContents {saveset {spname {}}} {
    
    # names will be the list of spectra to restore:
    
    set names [list]
    if {$spname ne ""} {
        set names $spname
    } else {
        set defs [$saveset listSpectra]
        foreach s $defs {
            lappend names [dict get $s name]
        }
    }
    foreach name $names {
        if {[$saveset hasChannels $name]}  {
            clear $name
            set channels [$saveset getChannels $name]
            foreach channel $channels {
                set xbin [lindex $channel 0]
                set ybin [lindex $channel 1]
                set value [lindex $channel 2]
                _restoreChannel $name $xbin $ybin $value
            }
        }
    }
    
}

##
# _getGateParams
#    Get parameters associated with a gate that has parameters.
#
# @param cmd  - database command
# @param gate - Gate dict so far.
# @return gate - Gate dict with parameters added.
#
proc _getGateParams {cmd gate} {
    set id [dict get $gate id ];   #  gate id.
    $cmd eval {
        SELECT name FROM parameter_defs
        INNER JOIN gate_parameters ON parameter_defs.id = gate_parameters.parameter_id
        WHERE gate_parameters.parent_gate = :id
    } {
        dict lappend gate parameters $name
    }
    return $gate
}
##
# _getParamsAnd1dPts
#
#    Flesh out the gate dict for a 1d gate.  1d gates have only x parameters
#   in their points.
#
# @param cmd   - database command.
# @param gate  - gate dict so far.
# @return dict - Filled in gate dictionary.
#
proc _getParamsAnd1dPts {cmd gate} {
    set id [dict get $gate id ];   #  gate id.
    
    $cmd eval {
        SELECT  x, y FROM gate_points WHERE gate_id = :id
    } {
        dict lappend gate points $x
    }
    
    set gate [_getGateParams $cmd $gate]
    return $gate
}
##
# _restoreSlice
#   Pull the parameter and point associated with a slice gate out of the
#   database and restore the slice:
#
# @param gate - The current gate dictionary.
#
proc _restoreSlice {gate} {
    
    
    set name [dict get $gate name]
    set param [dict get $gate parameters]
    set points [dict get $gate points]
    set low [dict get [lindex $points 0] x]
    set high [dict get [lindex $points 1] x]
    gate -new $name s [list $param [list $low $high]]
}
##
# _restoreGammaSlice
#    Restore gamma slice gates.  Very similar to _restoreSlice but the command
#    format is different.
#
# @param gate - Partial gate dict.
#
proc _restoreGammaSlice {gate} {
    
    set name [dict get $gate name]
    set params [dict get $gate parameters]
    set point [dict get $gate points]
    set low [dict get [lindex $point 0] x]
    set high [dict get [lindex $point 1] x]
    
    gate -new $name gs [list [list $low $high] $params]
}
##
# _getParamsAnd2dPts
#    Given a gate definition flesh out its dict to contain the parameters
#    and x/y coordinates of the gate points.  This is suitable for use with
#    b,c gc, gb gates (at least).
#
# @param cmd - database command.
# @param gate - the gate dict so far.
# @return dict - the fleshed out dict describing the gate.
#
proc _getParamsAnd2dPts {cmd gate} {
    set id [dict get $gate id ];   #  gate id.
    
    $cmd eval {
        SELECT  x, y FROM gate_points WHERE gate_id = :id
    } {
        dict lappend gate points [list $x $y]
    }
    set gate [_getGateParams $cmd $gate]

    return $gate
}
##
# _restore2dGate
#   Restore band or contours - note that the action is similar to that of
#  _restoreSlice but the points have x,y coords
#
# @param gate   - Gate dictionary as we have it so far.
#
proc _restore2dGate {gate} {
    
    set name [dict get $gate name]
    set type [dict get $gate type]
    set params [dict get $gate parameters]
    set pts    [dict get $gate points]
    
    # the pts are a list of dicts:
    
    set points [list]
    foreach pt $pts {
        set x [dict get $pt x]
        set y [dict get $pt y]
        lappend points [list $x $y]
    }
    
    gate -new $name $type [list {*}$params $points]
}
##
# _restore2dGammaGate
#   Restores a 2d gamma gate.  For reaons only I can guess the form of the
#   gate creation command is just that of c/b gates backwards, and the parameters
#   are a proper list.
#
# @param  gate - gate dict so far.
#
proc _restore2dGammaGate {gate} {
    
    set name [dict get $gate name]
    set type [dict get $gate type]
    set params [dict get $gate parameters]
    set pts    [dict get $gate points]
    set points [list]
    foreach pt $pts {
        set x [dict get $pt x]
        set y [dict get $pt y]
        lappend points [list $x $y]
    }

    gate -new $name $type [list $points $params]    
}
##
# _restoreCompoundGate
#   Restores a gate that's made up of other gates.
#
# @param gate - the dictionary so far.
#
proc _restoreCompoundGate {gate} {
    # Get the names of the gates associated with this one
    
    set id [dict get $gate id]
    set type [dict get $gate type]
    set gname [dict get $gate name]
    set gates [dict get $gate gates]
    

    gate -new $gname $type $gates
    
    
}
##
# _restoreMaskGate
#    Restore a bitmask gate:
#
# @param gate  - Gate dict so far.
#
proc _restoreMaskGate {gate} {
    
    
    set gname [dict get $gate name]
    set pname [dict get $gate parameters]
    set type [dict get $gate type]
    set mask [dict get $gate mask]
    
    gate -new $gname $type [list $pname $mask]
            
}
##
# _restoreGateDefs
#    Restore all gate definitions in a save set.
# @param saveset  - saveset instance command.
# @param sid  - The save set id to restore from.
#
proc _restoreGateDefs {saveset} {
    #  List the gates we need to restore.  The fact that they're ordered by the
    # PK ensures the save order is preserved and therefore the correct dependency
    # order is maintained.
    
    
    set gates [$saveset listGates]
    
    
    #  Now iterate over the gates doing the correct gate dependent restoration.
    
    foreach gate $gates {

        set type [dict get $gate type]
        if {$type eq "s"} {;      # Slice gate.
            _restoreSlice  $gate
        } elseif {$type in [list c b]} {
            _restore2dGate $gate
        } elseif {$type in [list gb gc]} {
            _restore2dGammaGate  $gate
        } elseif {$type eq "gs"} {
            _restoreGammaSlice $gate
        } elseif {$type in [list T F]} {
            set name [dict get $gate name]
            gate -new $name $type [list]
        } elseif {$type in [list * + -]} {
            _restoreCompoundGate $gate
        } elseif {$type in [list em am nm]} {
            _restoreMaskGate  $gate
        }
    }
}
##
# _restoreGateApplications
#    Restores the applications of gates to spectra.
#
# @param saveset - savest instance command.
# @param sid - The save set id.
#
proc _restoreGateApplications {saveset} {
    
    foreach application [$saveset listApplications] {
        set gate [dict get $application gate]
        set spec [dict get $application spectrum]
        apply $gate $spec
    }
}
##
# _restoreTreeVariables
#    Restore tree variables from a saveset:
#
# @param saveset  - saveset command instance
#
proc _restoreTreeVariables {saveset} {
    foreach def [$saveset listVariables] {
        set name [dict get $def name]
        set value [dict get $def value]
        set units [dict get $def units]
        treevariable -set $name $value $units
    }
}

##
# _getSpectrumId
#    Given a spectrum name and a saveset, return the spectrum id
#    else throw an error.
# @param cmd   - The database access command.
# @param sid   - The save set id in which the spectrum is stored.
# @param specname - name of the spectrum to look up.
# @return    - id of the spectrum in the spectrum_defs table.
# @throw error - if no matches.
#
proc _getSpectrumId {cmd sid specname} {
    set ids [list]
    $cmd eval {
        SELECT id FROM spectrum_defs
        WHERE save_id = :sid AND name = :specname
    } {
        lappend ids $id
    }
    # Highlander requirement:
    #     Not only can there be only 1, there must be exactly 1.
    
    if {[llength $ids] == 0} {
        error "No matching spectrum $specname in save set $sid"
    } elseif {[llength $ids] > 1} {
        error "BUG there's more than one id matchin $specname in $sid: $ids"
    }
    
    return $ids;                  # We know there can be only 1.
}
##
# _deleteContentsIfExists
#   If a spectrum has contents saved, they are removed.
#
# @param cmd - Database command.
# @param specid - Primary key of the spectrum whose contents we're deleing.
#                 note this already implies a save set.
# @note this is a silent No-op if there are no values to delete.
#
proc _deleteContentsIfExists {cmd specid} {
    $cmd eval {
        DELETE FROM spectrum_contents WHERE spectrum_id = :specid
    }
    
}
##
# _requireCompatible
#   Requires that a spectrum definition in SpecTcl be compatible with a
#   spectrum given by id in the database.  Compatibility means:
#
#   - The identified spectrum have the same spectrum types.
#   - The identified spectrum has the same number of axes and at least the
#     same number of bins on each axis.
#   - The named spectrum has the same set of parameters as the named spectrum.
#
# @param  cmd   - database command.
# @param  id    - Spectrum id (implies a save set id).
# @param  def   - SpecTcl definition to compare with.
# @throw error - if the two spectra ar not compatible.
#
proc _requireCompatible {cmd id def} {
    if {$def eq ""} {
        error "No such spectrum."
    }
    # Let's get the spectru, and its parameters first:
    
    set params [list]
    set specname [list]
    set sptype [list]
    $cmd eval {
        SELECT type, parameter_defs.name as pname, spectrum_defs.name as spname
        FROM spectrum_defs
        INNER JOIN spectrum_params ON spectrum_defs.id = spectrum_params.spectrum_id
        INNER JOIN parameter_defs   ON parameter_defs.id = spectrum_params.parameter_id
        WHERE spectrum_defs.id = :id
    } {
        set specname $spname
        set sptype   $type
        lappend params $pname
    }
    # Get the spectrum axis defs:
    
    set axes [list]
    $cmd eval {
        SELECT bins FROM axis_defs
        WHERE spectrum_id = :id
    } {
        lappend axes $bins;       # only care about bin count.
    }
    
    # Pick apart the spectrum def we were handed:
    
    set deftype [lindex $def 2]
    set defparams [lindex $def 3]
    set axislist [lindex $def 4]
    
    # Require the same spectrum type:
    
    if {$deftype ne $sptype} {
       error "Incompatible spectrum types db spectrum: $sptype other: $deftype"
    }
    # Require the same number and set of parameters:
    
    if {[llength $defparams] != [llength $params]} {
        error "Non-matching parameters.\n db spectrum: '$params' other '$defparams'"
    }
    # All parameters in the definition must be in the spectrum.
    
    foreach param $defparams {
        if {$param ni $params} {
            error "$param in the definition is not in the database spectrum parameters"
        }
    }
    # Require the same number of axes and the database bins must be at least
    # the number of bins in the definition.
    
    if {[llength $axislist] != [llength $axes]} {
        error "database spectrum and input spectrum don't have the same number of axes"
    }
    foreach axis $axislist dbbins $axes {
        set axisbins [lindex $axis 2]
        if {$axisbins > $dbbins} {
            error "The number of bins on the database spectrum, $dbbins is \
smaller than those of the named spectrum $axisbins"
        }
    }
    # we got here so it's all compatible.
}

##
# 
#-----------------------------------------------------------
#   Public interface
##
# Make the database schema... harmless if it already exists.
#
# @param fname - Name of the file in which the database wil be made.
#
proc makeSchema fname {
    DBTcl create $fname   
}
proc connect fname {
    return [DBTcl  connect $fname]
}
    
}
##
# Save a configuration.  Only one configuration of a given name can exist.
#  @param dbconnection  - the database insance command.
#  @param name - Name of the configuration.
#  @param spectra - optional.  If true, saves all spectrum contents as well.
#  @return object saveset command instance name.
#
proc saveConfig {dbconnection name {spectra 0}} {
    set timestamp [clock seconds]
    
    
    set saveset [$dbconnection createSaveset $name]
    
    _saveParameters       $saveset
    _saveSpectrumDefs     $saveset
    _saveGateDefinitions  $saveset
    _saveGateApplications $saveset
    _saveTreeVariables    $saveset
      if {$spectra} {
        _saveSpectraContents  $saveset
      }
      
    return $saveset
}


##
# listConfigs
#
#   List the known configurations
#   The result is a list of dicts with the keys:
#    -   id - the configuration id.
#    -   name - The configuration name.
#    -   time  - the [clock seconds] at which the configuration save was started.
#
# @param cmd - data base instance command.
# @return list of dicts -- see above.
proc listConfigs cmd {
    set result [list]
    
    set savenames [$cmd listSavesets]
    foreach name $savenames {
        set saveset [$cmd getSaveset $name]
        set info [$saveset info]
        lappend result $info
        $saveset destroy
    }
    return $result
}

##
# restoreConfig
#   Restore the configuration of a save set:
#
#  - parameter definitions are restored.
#  - spectrum definitions are restored.
#  - If requested, spectrum contents are also restored.
#
# @param cmd   - Database command.
# @param savename - Name of the save set.
# @param restoreSpectra - default false.
# @throw - error if save-name is not a save set.
#
#  Probably other errors will be thrown if cmd is a command for  a non
#  configuration database.
#
proc restoreConfig {cmd savename {restoreSpectra 0}} {
    
    set saveset [$cmd getSaveset $savename]
    
    
    #  Now restore the bits and pieces:
    
    # Restore parameters:
    
    _restoreParamDefs        $saveset
    _restoreSpectrumDefs     $saveset
    _restoreGateDefs         $saveset
    _restoreGateApplications  $saveset
    _restoreTreeVariables    $saveset
    
    if {$restoreSpectra} {
        _restoreSpectrumContents $saveset
    }
    
    $saveset destroy
}
##
# saveSpectrum
#   Saves a single spectrum's contents into the specified save set.
#
# @param sname Save set command instance.
# @param specname -- the spectrum name.
# @note the spectrum definition must already be in the save set.
# 
proc saveSpectrum {sname specname} {
    set channels [scontents $specname]
    $sname storeChannels $specname $channels
}
##
# restoreSpectrum
#   Restore the contents of a single spectrum from a save set in the database.
#
# @param sname   - Save set command instance.
# @param specname - Name of a saved spectrum to restore.
# @note - if there's an existing spectrum with that name it's silently deleted.
# @note - the spectrum restored is fully functional not a snapshot.
# @note - if there's no channel data associated with the spectrum you get an empty spectrum.
#
proc restoreSpectrum {sname specname} {
    _restoreSpectrumContents $sname $specname
}

##
# saveAllSpectrumContents
#   save all spectra currently defined into a save set.
#
# @param  sname - instance command
proc saveAllSpectrumContents {sname} {
    set speclist [$sname listSpectra]
    foreach spectrum $speclist {
        set name [dict get $spectrum name]
        $sname storeSpectrumContents $name [scontents $name]
    }

}
##
#  restoreAllSpectrumContents
#     Restores the channels in all spectra with saved data in a
#
# @param sname - save set instance name.
#
proc restoreAllSpectrumContents {sname} {
    _restoreSpectrumContents $sname
    
}

##
# listSavedSpectra
#    Lists the set of spectra saved in a configuration
#
# @param sname - configuration svae set instance command.
# @return list of spectrum names that have channels.
#
proc listSavedSpectra {sname} {
    set result [list]
    foreach spec [$sname listSpectra] {
        set name [dict get $spec name]
        if {[$sname hasChannels $name]} {
            lappend result $name
        }
    }
    
    return $result
}
##
# listRuns
#    List the runs in the database.
# @param cmd - database command.
# @return list of dicts.  Each dict has the following keys:
#         id     - the run id.
#         config -the id of the associated configuration.
#         number - the run number.
#         title  - the run title.
#         start_time - the run start time.
#         stop_time  - The run stop time (not provided if null).
# @note the times are in [clock seconds] representation.
#
proc listRuns {cmd} {
#    set result [list]
#    $cmd eval {
#        SELECT id, config_id, run_number, title, start_time, stop_time
#        FROM runs
#    } {
#        set item [dict create id $id config $config_id \
#                  number $run_number title $title \
#                  start_time $start_time]
#        if {$stop_time ne ""} {
#           dict set item stop_time $stop_time
#        }
#        lappend result $item
#    }
#    return $result
    
}

##
# hasRun
#  Returns true if the configuration specified has an associated
#  run.
#
# @param cmd - database command.
# @param confid - configuration id.
# @return bool
#
proc hasRun {cmd confid} {
#    $cmd eval {
#        SELECT COUNT(*) as result FROM runs
#            WHERE config_id = $confid
#    } {
#        return $result
#    }
}

##
# getRunInfo
#   Get information about a run associated with a configuration
#   id.
#
# @param cmd -- database command
# @param conf - Configuration id.
# @return dict (possibily empty if there is no run
#              See listRuns for the keys/values to this dict.
# @retval empty dict if there's no associated run.
#
proc getRunInfo {cmd conf} {
#    set result [dict create]
#    $cmd eval {
#        SELECT id, config_id, run_number, title, start_time, stop_time
#        FROM runs WHERE config_id = $conf
#    } {
#        set result [dict create                            \
#            id $id config $config_id number $run_number title $title \
#            start_time $start_time
#        ]
#        if {$stop_time ne ""} {
#            dict set result stop_time $stop_time
#       }
#    }
#    
#    return $result
}
##
# getScalers
#   @param cmd   - database command.
#   @param run  - Rim id.
#   @returns a list of dicts.  Each dict has the following keys:
#            -  start - Seconds into the run when a scaler accumulation started
#            -  stop  - Seconds into the run when a scaler accumulation ended.
#            -  timestamp - Timestamp at which the readout was done.
#            -  channels  - This is a list of pairs.  The first element
#                           of each pair is the channel number/vaule of a scaler

proc getScalers {cmd run} {
    #set result [list]
    #set lastid -1
    #set currentDict [list]
    #db eval {
    #    SELECT scaler_readouts.id as id, run_id, source_id,
    #           start_offset, stop_offset, divisor, clock_time, channel, value
    #        FROM scaler_readouts
    #        INNER JOIN scaler_channels
    #              ON scaler_channels.readout_id = scaler_readouts.id
    #        WHERE run_id = $run} {
    #    # If id changed it's a new readout:
    #    # Need a new dict to add.
    #    
    #    
    #    if {$id != $lastid}  {
    #        if {$currentDict ne ""} {
    #            lappend result $currentDict;   #if there's a prior add it.
    #        }
    #        set lastid $id
    #        
    #        # Create the new readout dict.
    #        
    #        set currentDict [dict create \
    #            start $start_offset stop $stop_offset divisor $divisor timestamp $clock_time \
    #            sourceid $source_id                                           \
    #            channels [list]]
    #    }
    #    #  Add the channel info:
    #    
    #    dict lappend currentDict channels [list $channel $value]
    #}
    #if {$currentDict ne "" } {
    #    lappend result $currentDict
    #}
    #
    #return $result
}

}
