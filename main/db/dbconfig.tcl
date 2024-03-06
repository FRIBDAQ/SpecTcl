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
package provide dbconfig 2.0
package require SpecTclDB
package require sqlite3
namespace eval dbconfig {
#----------------------------------------------------------
#  Private procs.



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
            # Safest to re-use the old parameter number:
            
            set number [lindex [lindex [parameter -list $name] 0] 1]
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
        set cmd [_makeSpectrumCreateCommand $def]
        eval $cmd
        
    }
}
##
#  _makeSpectrumCreateCommand
#   Given a spectrum definition dict, produce
#   the command to recreate that spectrum.
#   caller cna then eval the return value from
#   this proc.
proc _makeSpectrumCreateCommand {def} {
    set name [dict get $def name]
    set type [dict get $def type]
    set cmd [list spectrum -new $name]
    set axisDefs [dict get $def axes]
        lappend cmd $type
        # How we create the parameters part of the definition
        # depends on if the x/y parameters keys exist
        # and at least one of them has non-zero length
        # and the spectrum type.
        set params [dict get $def parameters]
        if {[dict exists $def xparameters ] && [dict exists $def yparameters]} {
            set xpars [dict get $def xparameters]
            set ypars [dict get $def yparameters]
            if {[llength $xpars] > 0 || [llength $ypars] > 0} {
                #  Now it depends on the spectrum type:
                
                if {$type in [list 1 g1 g2 s b]} {
                    set params $xpars 
                    if {$type in [list 1 g1 s b]} {
                        set axisDefs [list [lindex $axisDefs 0]];   # rustogramer can give extra exes.
                    }
                } elseif {$type in [list 2 S]} {
                    set params [list \
                        $xpars $ypars
                    ]
                } elseif {$type eq "g1" || $} {
                    set params $xpars
                } elseif {$type eq "m2"} {
                    # Interleave x/y parameters
                    set params [list]
                    foreach x $xparams y $yarams {
                        lappend params $x $y
                    }
                }
            }
            # For spectra we don't know what to do with, leave params alone!

        } 
        lappend cmd $params
        
        set axiscmd [list]
        foreach adef $axisDefs {
            set low [dict get $adef low]
            set high [dict get $adef high]
            set bins [dict get $adef bins]
            lappend axiscmd [list $low $high $bins]
        }
        set dtype [dict get $def datatype]
        if {$dtype eq "f64"} {
            set dtype long;      # Came from Rustogramer.
        }
    
        lappend cmd $axiscmd $dtype
        return $cmd

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
##
# connect
#  Given a database, create a command instance that operates on
#  that database.
#
# @param fname = path to the file containng the database.
# @return command - Command to use to mainpulate the dtabase.
#                    (Database instance command)
# @note at some point the caller should excecute the destroy method
#       of the returned command ensemble.
#
proc connect fname {
    return [DBTcl  connect $fname]
}
##
# openSaveSet
#    Creates a saveset instance command by looking up an existing saveset
#    given a database instance command.
#
#  @param dbcmd - database instance command (e.g. from connect).
#  @param name  - name of the saveset to lookup.
#  @return command - command ensemble representing the saveset.
#         this is a parameter to many dbconfig procs.
#
# @note at some point the caller should invoke the destroy method of the
#       returned command ensemble.
#
proc openSaveSet {dbcmd name} {
    return [$dbcmd getSaveset $name]
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
# save a configuration using the rustogramer saveset name:
#
proc saveForRustogramer {dbconnection} {
    return [saveConfig $dbconnection rustogramer_gui 0]
}


##
# listConfigs
#
#   List the known configurations
#   The result is a list of dicts with the keys:
#    -   id - the configuration id.
#    -   name - The configuration name.
#    -   timestamp  - the [clock seconds] at which the configuration save was started.
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
# restoreRustogramer 
#   Restore the rustogramer_gui saveset.. convenience 
#   proc for SpecTcl/Rustogramer definition interchanges
# 
#  Throws an error if there's no such saveset.
#
proc restoreRustogramer {cmd} {

    return [restoreConfig $cmd rustogramer_gui 0]
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
#
#         config -the name of the associated configuration.
#         number - the run number.
#         title  - the run title.
#         start_time - the run start time.
#         stop_time  - The run stop time (not provided if null).
# @note the times are in [clock seconds] representation.
#
proc listRuns {cmd} {
    
    return [$cmd listRuns]
}

##
# hasRun
#  Returns true if the configuration specified has at least one
#  associated run.
#

# @param saveset- savset instance command.
# @return bool - if there is at least one run associated with this save set.
#
proc hasRun {saveset} {
    set  runs [$saveset listRuns]

    return [expr {[llength $runs] > 0}]
}

##
# getRunInfo
#   Get information about runs associated with a saveset  instance command.
#   
#
# @param saveset - saveset instance command
# @return list of dicts (possibily empty if there are no runs
#              See listRuns for the keys/values to these dicts.
# @retval empty dict if there's no associated run.
#
proc getRunInfo {saveset} {
    return [$saveset getRunInfo]    
}
##
# getScalers
#   @param saveset   - saveset instance command.
#   @param run  - Run Number.
#   @returns a list of dicts.  Each dict has the following keys:
#            -  sourceid
#            -  start - Seconds into the run when a scaler accumulation started
#            -  stop  - Seconds into the run when a scaler accumulation ended.
#            -  divisor - What to divide start/stop by to get seconds.db
#            -  timestamp - Timestamp at which the readout was done.
#            -  channels  - This is a list of of channel values in channel order
proc getScalers {saveset run} {
    return [$saveset getScalers $run]

 }

}
