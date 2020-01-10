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
# @param cmd - data base command.
# @param sid - Save set id.
#
#   - If a parameter has a tree parameter definition it is saved
#     fully, otherwise only the parameter name and number are saved.
#
proc _saveParameters {cmd sid} {
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
            
            $cmd eval {
                INSERT INTO parameter_defs
                    (save_id, name, number, low, high, bins, units)
                    VALUES(:sid, :name, :number, :low, :high, :bins, :units)
            }
            
        } else {
            # only save name and number:
            
            $cmd eval {
                INSERT INTO parameter_defs
                    (save_id, name, number)
                    VALUES(:sid, :name, :number)
            }
        }
    }
}
##
# _addSpecParam
#    Adds a parameter to the list of parameters used by the spectrum. These
#
# @param cmd  - Database command.
# @param specid - Spectrum id
# @param parname - parameter name.
# @param sid     - save set id under which the parameter was saved.
#
proc _addSpecParam {cmd specid parname sid} {
    # Get the parameter id from the parameter_defs table:
    
    set pid "";             #if not defined it's an error of course.
    $cmd eval {
        SELECT id FROM parameter_defs WHERE save_id = :sid AND name = :parname
    } {
        set pid $id
    }
    if {$pid eq ""} {
        error "Spectrum defined on nonexistent parameter: $parname"
    } else {
        $cmd eval {
            INSERT INTO spectrum_params (spectrum_id, parameter_id)
            VALUES(:specid, :pid)
        }
    }
}
##
#  _addSpecAxis
#
#   Adds an axis definition for a spectrum.
#
# @param cmd - database command.
# @param specid - spectrum id (implies a save set id).
# @param axis   - axis definition (3 element low, high bints list).
#
proc _addSpecAxis {cmd specid axis} {

    set low [lindex $axis 0]
    set high [lindex $axis 1]
    set bins [lindex $axis 2]
    
    $cmd eval {
        INSERT INTO axis_defs
        (spectrum_id, low, high, bins)
        VALUES(:specid, :low, :high, :bins)
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
# @param cmd - data base command.
# @param sid - save set id.
#
proc _saveSpectrumDefs {cmd sid} {
    foreach def [spectrum -list *] {
        set name [lindex $def 1]
        set type [lindex $def 2]
        set params [lindex $def 3]
        set axes   [lindex $def 4]
        set datatype [lindex $def 5]
        
        # Create the root entr in spectrum_defs  its'id is used to make the others:

        $cmd eval {
            INSERT INTO spectrum_defs
            (save_id, name, type, datatype)
            VALUES (:sid, :name, :type, :datatype)
        }
        set specid [$cmd last_insert_rowid]
        foreach pname $params {
            _addSpecParam $cmd $specid $pname $sid
        }
        foreach axis $axes {
            _addSpecAxis $cmd $specid $axis
        }
    }
}
##
# _addLeadingParameters
#    Adds a fixed number of parameters to the gate from a description
#    when those parameters lead the description:
#
# @param cmd   - Database command.
# @param gid   - Gate id these parameters belong with.
# @param sid   - save set id
# @param descr - Gate description string
# @param n     - Number of parameters.
#
proc _addLeadingParameters {cmd gid sid descr n} {
    # We need to look up the parameters to get their ids
    
    
    set names [lindex $descr 0];          # first is a list of parameters.
    foreach name $names {
        set pid ""
        $cmd eval {
            SELECT id FROM  parameter_defs
            WHERE save_id = :sid AND name = :name
        } {
            set pid $id
        }
        if {$pid eq ""} {
            error "Adding a gate parameter : $name is not defined as a parameter"
        }
        $cmd eval {
            INSERT INTO gate_parameters (parent_gate, parameter_id)
                VALUES (:gid, :pid)
        }
    }
}
##
# _addTrailingParameters
#    Adds parameters that are in a list that is the last element of the gate
#    Description
#
# @param cmd    - database command.
# @param gid    - gate id of the ownning gate.
# @param sid    - Save id.
# @param descr  - Gate description
#
proc _addTrailingParameters {cmd gid sid descr} {
    
    set names [lindex $descr end];        # list of parameter names.
    foreach name $names {
        set pid  ""
        $cmd eval {
            SELECT id FROM parameter_defs
            WHERE save_id = :sid AND  name = :name
        } {
            set pid $id
        }
        if {$pid eq ""} {
            error "Adding a gate parameter : $name is not a defined paramter"
        } else {
            $cmd eval {
                INSERT INTO gate_parameters (parent_gate, parameter_id)
                    VALUES (:gid, :pid)
            }
        }
    }
}
##
# _addTrailingPoints
#
#   Add points to a primitive gate
#
# @param cmd - database command
# @param gid - gate id
# @param descr - the gate description.
#
proc _addTrailingPoints {cmd gid descr} {
    set points [lrange $descr 1 end];    #extract the points.
    foreach point $points {
        set x [lindex $point 0]
        set y [lindex $point 1]
        
        $cmd eval {
            INSERT INTO gate_points (gate_id, x, y)
            VALUES (:gid, :x, :y)
        }
    }
}
##
# _addGateMask
#    Add the mask data for a bitmask gate:
#
# @param cmd   - The database command.
# @param gid   - Gate id.
# @param descr - Gate description.  The second element is the mask
#
proc _addGateMask {cmd gid descr} {
    set mask [lindex $descr 1]
    $cmd eval {
        INSERT INTO gate_masks (parent_gate, mask)
        VALUES (:gid, :mask)
    }
}
##
# _addLeadingPoints
#    Adds points for gate descriptions where the points are a list that is the
#    first element of the gate description.
#
#  @param cmd     - Database command
#  @param gate_id - Gate id.
#  @param descr   - Gate description.
#
proc _addLeadingPoints {cmd gate_id descr} {
    set points [lindex $descr 0]

    foreach point $points {
        set x [lindex $point 0]
        set y [lindex $point 1]
        
        $cmd eval {
            INSERT INTO gate_points (gate_id, x, y)
            VALUES (:gate_id, :x, :y)
        }
    }
}
##
# _addLeadingPoint
#    Save the two x parameter of a gamma slice.
#
#  @param cmd   - Database Command.
#  @param gid   - Gate id.
#  @param descr - Gate description.
#
proc _addLeadingPoint {cmd gid descr} {
    set point [lindex $descr 0]
    set x     [lindex $point 0]
    set y     [lindex $point 1]

    foreach x $point {    
        $cmd eval {
            INSERT INTO gate_points (gate_id, x)
            VALUES (:gid, :x)
        }
    }
}
##
# _addComponentGates
#    For compound gates, write the ids of the component gates.  Note that
#    We're assuming that the gates we depend on have already been written
#    We do  a sanity check to ensure this was the case.
#
# @param cmd    - Database command.
# @param sid    - saveset id.
# @param gid    - my gate id.
# @param descr  - Description which is a list of dependent gates.
#
proc _addComponentGates {cmd sid gid descr} {
    set depNames [list]
    
    # Can't just join strings -- need to surround them with ""'s to make SQL
    # happy.
    
    foreach gateName $descr {
        lappend depNames "\"$gateName\""
    }
    set depNames [join $depNames ,]
    
    set depIds [list]

    $cmd eval "                            
        SELECT id FROM gate_defs           
        WHERE saveset_id = $sid            
        AND   name IN ($depNames)
    "   {
        lappend depIds $id
    }
    
    if {[llength $depIds] != [llength $descr]} {
        error "BUG - trying to write a gate whose dependent gates were not yet written."
    }
    foreach id $depIds {
        $cmd eval {
            INSERT INTO component_gates (parent_gate, child_gate)
            VALUES (:gid, :id)
        }
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
        foreach gate $dependencies {
            if {$gate ni $written} {
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
# _add1dSlicePoint
#   Adds a 1d point for a slice.  The point is a single list elemnt with
#   low, high limts that are only x coordinates:
#
# @param cmd - database command.
# @param gid  - id of the gate.
# @param desc  - gate description
#
#
proc _add1dSlicePoint {cmd gid desc} {
    set lims [lindex $desc 1]
    foreach lim $lims {
        $cmd eval {
            INSERT INTO gate_points (gate_id, x)
            VALUES (:gid, :lim)
        }
    }
}
##
# _saveGateDefinitions
#    Save definitions of gates.  This one has tones of special cases depending
#    on the gate type.
#
# @param cmd - database command
# @param sid - save id.
#
proc _saveGateDefinitions {cmd sid} {
    set gates [gate -list]
    
    # Strip the gate of its id (element1)
    
    set strippedGates [list]
    foreach gate $gates {
        lappend strippedGates [lreplace $gate 1 1]
    }
    set gates $strippedGates
    
    #  All gates have a name and type that must be entered.
    #  This gets entered in the gate_defs table.
    #
    # TODO:  The gates must be re-ordered so that compound gates are only written
    #        Once their component gates are written.
    
    set gates [_ReorderGates $gates]
    foreach gate $gates {
        set name [lindex $gate 0]
        set type [lindex $gate 1]
        set descr [lindex $gate 2]
        
        $cmd eval {
            INSERT INTO gate_defs (saveset_id, name, type)
            VALUES (:sid, :name, :type)
        }
        set gate_id [$cmd last_insert_rowid];    # for foreign keys.
        
        # Several gate types have parameter names in the leading part of
        # the description:
        
        if {$type in [list b c]} {
            _addLeadingParameters $cmd $gate_id $sid $descr 2
            _addTrailingPoints    $cmd $gate_id $descr
        }
        if {$type in [list s em am nm]} {
            _addLeadingParameters $cmd $gate_id $sid $descr 1
            if {$type eq "s"} {
                _add1dSlicePoint $cmd $gate_id $descr
            } else {
                _addGateMask $cmd $gate_id $descr
            }
        }
        if {$type in [list gs gb gc]} {
            _addTrailingParameters $cmd $gate_id $sid $descr
            if {$type in [list gc gb]} {
                _addLeadingPoints      $cmd $gate_id $descr
            } else {
                _addLeadingPoint $cmd $gate_id $descr
            }
        }
        # Compound gates (note that T/F gates have no additional data)
        
        if {$type in [list * + -]} {
            _addComponentGates $cmd $sid $gate_id $descr
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
# @param cmd -- The gate command.
# @param sid -- The save set id.
#
proc _saveGateApplications {cmd sid} {
    set applications [apply -list]
    
    # rather than looking things up one application at  a time build hashes
    # that take names to ids for spectra and gates:
    
    array set spectra [list]
    array set gates [list]
    
    $cmd eval {
        SELECT id, name FROM spectrum_defs WHERE save_id = :sid
    } {
        set spectra($name) $id
    }
    $cmd eval {
        SELECT id, name FROM gate_defs WHERE saveset_id = :sid
    } {
        set gates($name) $id
    }
    #  Now run through the applications. Note that if the gate is named -TRUE-
    # the spectrum is ungated and we won't list an application for it:
    
    foreach application $applications {
        set spname [lindex $application 0]
        set gname  [lindex [lindex $application 1] 0]
        if {$gname ne "-TRUE-"} {
            set spid $spectra($spname)
            set gid  $gates($gname)
            
            $cmd eval {
                INSERT INTO gate_applications (spectrum_id, gate_id)
                VALUES (:spid, :gid)
            }
        }
    }
    
}
##
# _saveTreeVariables
#    Saves tree variables/names.
# @param cmd - Database command.
# @param sid - Save set id.
#
proc _saveTreeVariables {cmd sid} {
    set defs [treevariable -list]
    foreach def $defs {
        set name [lindex $def 0]
        set value [lindex $def 1]
        set units [lindex $def 2]
        
        $cmd eval {
            INSERT INTO treevariables (save_id, name, value, units)
            VALUES (:sid, :name, :value, :units)
        }
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
# @param cmd - database command
# @param sid - the save id.
# @note the assumption is that the spectrum definitions have already been saved.
#
proc _saveSpectraContents {cmd sid} {
    # Get a list of spectrum names in this save id:
    
    set specs [list];   # will be the pairs of db ids and spectcl spectrum names.
    $cmd eval {
        SELECT id,name FROM spectrum_defs WHERE save_id = :sid
    } {
        lappend specs [list $id $name]
    }
    foreach spec $specs {
        set data [scontents [lindex $spec 1]]
        _saveSpectrumChans $cmd [lindex $spec 0] $data
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
# @param cmd - DB command name.
# @param saveid- Save set id.
#
proc _restoreParamDefs {cmd saveid} {
    $cmd eval {
        SELECT name, number, low, high, bins, units FROM parameter_defs
        WHERE save_id = :saveid
    } {
        set existingDef [parameter -list $name]
        if {[llength $existingDef] > 0} {
            parameter -delete $name
        }
        parameter -new $name $number
        
        #  Do we need to create/restore the tree parameter info?
        
        if {($low ne "") && ($high ne "") && ($bins ne "") } {
            _restoreTreeParam $name $low $high $bins $units
        }
    }
}
##
# _restoreSpectrumDefs
#    Restores the spectrum definitions for a saveset.  Any existing spectrum
#    with the same name is deleted first.
#
# @param cmd - database command.
# @param sid - save set id.
# @param spid - if provided the id of the single specstrum id we want
#               to restore.
#
proc _restoreSpectrumDefs {cmd sid {spid -1}} {
    # Get the spectrum top level defs.  These will go into an array of
    # dicts indexed by the spectrum id
    
    array set topdefs [list];          # Array of top level dicts.
    set idlist [list]
    if {$spid == -1} {
        $cmd eval {
            SELECT * from spectrum_defs  WHERE save_id = :sid
            ORDER BY id ASC
        } {
            set value [dict create id $id name $name type $type datatype $datatype]
            set topdefs($id) $value
            lappend idlist $id
        }
    } else {
        $cmd eval {
            SELECT * from spectrum_defs WHERE id = :spid
        } {
            set value [dict create id $id name $name type $type datatype $datatype]
            set topdefs($id) $value
            lappend idlist $id

        }
        
    }
    
    set idlist [join $idlist ,]
    
    # Now get the parameter names for each spectrum.
    
    $cmd eval "
        SELECT spectrum_id, name FROM spectrum_params
        INNER JOIN parameter_defs ON  parameter_id = parameter_defs.id 
        WHERE spectrum_id IN ($idlist)
        ORDER BY spectrum_params.id ASC
    "   {
        dict lappend topdefs($spectrum_id) parameters $name
    }
    # Now the axis definitions:
    
    $cmd eval "
        SELECT spectrum_id, low, high, bins FROM axis_defs
        WHERE spectrum_id IN ($idlist)
        ORDER BY id ASC
    " {
        dict lappend topdefs($spectrum_id) axes [list $low $high $bins]
    }
    # Now we can run over the spectra, deleting existing ones and
    # Creating the ones in the definition.
    
    set idlist [split $idlist ,];   #Maybe faster than array names?
    foreach id $idlist {
        set def $topdefs($id)
    
        set name [dict get $def name]
        if {[llength [spectrum -list $name]] != 0 } {
            spectrum -delete $name
        }
        # Construct the spectrum definition command:
        
        set cmd [list spectrum -new $name]
        lappend cmd [dict get $def type] [dict get $def parameters]
        lappend cmd [dict get $def axes] [dict get $def datatype]

        eval $cmd
    }
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
    if {$ybin ne ""} {
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
# @param cmd - database command.
# @param sid - save-set id.
#
proc _restoreSpectrumContents {cmd sid {spid -1}} {
    
    array set haveCleared [list]
    if {$spid == -1} {
        
        
        $cmd eval {
            SELECT name, xbin, ybin, value FROM spectrum_defs
            INNER JOIN spectrum_contents ON spectrum_defs.id = spectrum_id
            WHERE save_id= :sid
        } {
            # On the first occurence of a spectrum name, clear it:
            if {[array names haveCleared $name] == ""} {
                clear $name
                set haveCleared($name) 1
            }
            _restoreChannel $name $xbin $ybin $value

        }
    } else {
         $cmd eval {
            SELECT name, xbin, ybin, value FROM spectrum_defs
            INNER JOIN spectrum_contents ON spectrum_defs.id = spectrum_id
            WHERE save_id= :sid AND spectrum_id = :spid
        } {
            if {[array names haveCleared $name] == ""} {
                clear $name
                set haveCleared($name) 1
            }
            _restoreChannel $name $xbin $ybin $value

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
# @param cmd  - The database command.
# @param gate - The current gate dictionary.
#
proc _restoreSlice {cmd gate} {
    set gate [_getParamsAnd1dPts $cmd $gate]
    
    set name [dict get $gate name]
    set param [dict get $gate parameters]
    set point [dict get $gate points]
    gate -new $name s [list $param $point]
}
##
# _restoreGammaSlice
#    Restore gamma slice gates.  Very similar to _restoreSlice but the command
#    format is different.
#
# @param cmd  - Database command.
# @param gate - Partial gate dict.
#
proc _restoreGammaSlice {cmd gate} {
    set gate [_getParamsAnd1dPts $cmd $gate]
    
    set name [dict get $gate name]
    set params [dict get $gate parameters]
    set point [dict get $gate points]
    
    gate -new $name gs [list $point $params]
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
# @param cmd    - database command.
# @param gate   - Gate dictionary as we have it so far.
#
proc _restore2dGate {cmd gate} {
    set gate [_getParamsAnd2dPts $cmd $gate]
    
    set name [dict get $gate name]
    set type [dict get $gate type]
    set params [dict get $gate parameters]
    set pts    [dict get $gate points]
    
    gate -new $name $type [list {*}$params $pts]
}
##
# _restore2dGammaGate
#   Restores a 2d gamma gate.  For reaons only I can guess the form of the
#   gate creation command is just that of c/b gates backwards, and the parameters
#   are a proper list.
#
# @param  cmd  - Database command.
# @param  gate - gate dict so far.
#
proc _restore2dGammaGate {cmd gate} {
    set gate [_getParamsAnd2dPts $cmd $gate]
    
    set name [dict get $gate name]
    set type [dict get $gate type]
    set params [dict get $gate parameters]
    set pts    [dict get $gate points]


    gate -new $name $type [list $pts $params]    
}
##
# _restoreCompoundGate
#   Restores a gate that's made up of other gates.
#
# @param cmd  - The database command.
# @param gate - the dictionary so far.
#
proc _restoreCompoundGate {cmd gate} {
    # Get the names of the gates associated with this one
    
    set id [dict get $gate id]
    set type [dict get $gate type]
    set gname [dict get $gate name]
    set gates [list]
    $cmd eval {
        SELECT name FROM gate_defs
        INNER JOIN component_gates ON component_gates.child_gate = gate_defs.id
        WHERE parent_gate = :id
    } {
        lappend gates $name
    }
    

    gate -new $gname $type $gates
    
    
}
##
# _restoreMaskGate
#    Restore a bitmask gate:
#
# @param cmd   - database command.
# @param gate  - Gate dict so far.
#
proc _restoreMaskGate {cmd gate} {
    set gate [_getGateParams $cmd $gate];            # add the parameters to the dict.
    set id [dict get $gate id]
    $cmd eval {
        SELECT mask from gate_masks WHERE parent_gate = :id
    } {
        dict lappend gate mask $mask
    }
    
    set gname [dict get $gate name]
    set pname [dict get $gate parameters]
    set type [dict get $gate type]
    set mask [dict get $gate mask]
    
    gate -new $gname $type [list $pname $mask]
            
}
##
# _restoreGateDefs
#    Restore all gate definitions in a save set.
# @param cmd  - database command
# @param sid  - The save set id to restore from.
#
proc _restoreGateDefs {cmd sid} {
    #  List the gates we need to restore.  The fact that they're ordered by the
    # PK ensures the save order is preserved and therefore the correct dependency
    # order is maintained.
    
    set gates [list]
    $cmd eval {
        SELECT id, name, type FROM gate_defs WHERE saveset_id = :sid
        ORDER BY id ASC
    } {
        lappend gates [dict create id $id name $name type $type]
    }
    
    #  Now iterate over the gates doing the correct gate dependent restoration.
    
    foreach gate $gates {

        set type [dict get $gate type]
        if {$type eq "s"} {;      # Slice gate.
            _restoreSlice $cmd $gate
        } elseif {$type in [list c b]} {
            _restore2dGate $cmd $gate
        } elseif {$type in [list gb gc]} {
            _restore2dGammaGate  $cmd $gate
        } elseif {$type eq "gs"} {
            _restoreGammaSlice $cmd $gate
        } elseif {$type in [list T F]} {
            set name [dict get $gate name]
            gate -new $name $type [list]
        } elseif {$type in [list * + -]} {
            _restoreCompoundGate $cmd $gate
        } elseif {$type in [list em am nm]} {
            _restoreMaskGate $cmd $gate
        }
    }
}
##
# _restoreGateApplications
#    Restores the applications of gates to spectra.
#
# @param cmd - database command.
# @param sid - The save set id.
#
proc _restoreGateApplications {cmd sid} {
    
    $cmd eval {
        SELECT spectrum_defs.name AS spname, gate_defs.name AS gname
        FROM gate_applications
        INNER JOIN spectrum_defs ON gate_applications.spectrum_id = spectrum_defs.id
        INNER JOIN gate_defs  ON gate_applications.gate_id = gate_defs.id
        WHERE spectrum_defs.save_id = :sid
        ORDER BY gate_applications.id ASC
    } {
        apply $gname $spname
    }
}
##
# _restoreTreeVariables
#    Restore tree variables from a saveset:
#
# @param cmd - database comand.
# @param sid - The save set id.
#
proc _restoreTreeVariables {cmd sid} {
    $cmd eval {
        SELECT name, value, units FROM treevariables
        WHERE save_id = :sid
    } {
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
#   @#param cmd - database command.
#
proc makeSchema cmd {
    
    # Multiple save sets:
    
    $cmd eval {
        CREATE TABLE IF NOT EXISTS  save_sets 
        (id  INTEGER PRIMARY KEY,
         name TEXT UNIQUE,
         timestamp INTEGER)
    }
    # Parameter definitions
    
    $cmd eval {
        CREATE TABLE IF NOT EXISTS parameter_defs
        (id      INTEGER PRIMARY KEY,                    
         save_id INTEGER NOT NULL,  -- foreign key to save_sets.id
         name    TEXT NOT NULL,
         number  INTEGER NOT NULL,
         low     REAL,
         high    REAL,
         bins    INTEGER,
         units   TEXT)
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS pdef_save_id ON parameter_defs (save_id)
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS pdef_name ON parameter_defs (name)
    }
    # Spectrum definitions:
    
    $cmd eval {
        CREATE TABLE IF NOT EXISTS spectrum_defs
        (id      INTEGER PRIMARY KEY,
         save_id INTEGER NOT NULL,     -- Foreign key to save_sets.id
         name    TEXT NOT NULL,
         type    TEXT NOT NULL,
         datatype TEXT NOT NULL
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS sdef_save_id ON spectrum_defs (save_id)
    }
    
    
                 # Axes a spectrum has....
    $cmd eval {
        CREATE TABLE IF NOT EXISTS axis_defs
        (
            id           INTEGER PRIMARY KEY,
            spectrum_id  INTEGER NOT NULL,  -- FK to spectrum_defs.id
            low          REAL NOT NULL,
            high         REAL NOT NULL,
            bins         INTEGER NOT NULL
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS adef_specid ON axis_defs (spectrum_id)
    }
                   # Parameter a spectrumis defined on.
    $cmd eval {
        CREATE TABLE IF NOT EXISTS spectrum_params
        (
            id          INTEGER PRIMARY KEY,
            spectrum_id INTEGER NOT NULL,    -- FK to spectrum_defs.id
            parameter_id INTEGER NOT NULL    -- Fk to parameter_defs.id
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS sparams_spectrum_id ON spectrum_params (spectrum_id)
    }
    
    # Spectrum contents:
    
    $cmd eval {
        CREATE TABLE IF NOT EXISTS spectrum_contents
        (
            id             INTEGER PRIMARY KEY,
            spectrum_id    INTEGER NOT NULL,      -- FK to spectrum_defs.id.
            xbin           INTEGER NOT NULL,
            ybin           INTEGER,
            value          INTEGER NOT NULL
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS scontents_spectrum_id ON spectrum_contents (spectrum_id)
    }

    # Gate definitions
    
    $cmd eval {
        CREATE TABLE IF NOT EXISTS gate_defs
        (
            id          INTEGER PRIMARY KEY,
            saveset_id  INTEGER NOT NULL,   -- FK to save_sets.id
            name        TEXT NOT NULL,
            type        TEXT NOT NULL
        )
    }
        # Primitive gates have points:
    $cmd eval {
        CREATE TABLE IF NOT EXISTS gate_points
        (
            id          INTEGER PRIMARY KEY,
            gate_id     INTEGER NOT NULL,           -- FK to gate_defs.id
            x           REAL,                       -- Point x coordinate.
            y           REAL                        -- Point y coords.
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS gate_points_gatidx ON gate_points (gate_id)
    }
        # Compound gates depend on other gates:
        
    $cmd eval {
        CREATE TABLE IF NOT EXISTS component_gates
        (
            id          INTEGER PRIMARY KEY,
            parent_gate INTEGER NOT NULL,      -- gate_defs.id of owner.
            child_gate  INTEGER NOT NULL       -- Gate parent_gate depends on.
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS component_gates_parentidx
            ON component_gates (parent_gate)
    }
        # Some gates need parameter ids:
    $cmd eval {
        CREATE TABLE IF NOT EXISTS gate_parameters
        (
            id   INTEGER PRIMARY KEY,
            parent_gate INTEGER NOT NULL,     -- FK to gate_defs.id
            parameter_id INTEGER NOT NULL     -- FK for parameter_defs.id
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS gate_params_parentidx
            ON gate_parameters (parent_gate)
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS gate_params_paramidx
            ON gate_parameters (parameter_id)
    }
      # some gates have masks:
    $cmd eval {
        CREATE TABLE IF NOT EXISTS gate_masks
        (
            id          INTEGER PRIMARY KEY,
            parent_gate INTEGER NOT NULL,
            mask        INTEGER NOT NULL
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS gate_mask_parentix ON gate_masks (parent_gate)
    }
    #   Gate applications:
    
    $cmd eval {
        CREATE TABLE IF NOT EXISTS gate_applications (
            id                INTEGER PRIMARY KEY,
            spectrum_id       INTEGER NOT NULL,      -- fk to spectrum_defs
            gate_id           INTEGER NOT NULL       -- fk to gate_defs
        )
    }
    #  Tree variables:
    
    $cmd eval {
        CREATE TABLE IF NOT EXISTS treevariables (
            id             INTEGER PRIMARY KEY,
            save_id        INTEGER NOT NULL,
            name           TEXT NOT NULL,
            value          DOUBLE NOT NULL,
            units          TEXT
        )
    }
    $cmd eval {
        CREATE INDEX IF NOT EXISTS treevariables_saveidx ON treevariables (save_id)
    }
}
##
# Save a configuration.  Only one configuration of a given name can exist.
#
#  @param cmd  - the databsae command.
#  @param name - Name of the configuration.
#  @param spectra - optional.  If true, saves all spectrum contents as well.
#  @return int - The id of the save's root record.
#
proc saveConfig {cmd name {spectra 0}} {
    set timestamp [clock seconds]
    
    $cmd transaction {
        $cmd eval {INSERT INTO save_sets (name, timestamp) VALUES(:name, :timestamp)}
        set save_id [$cmd last_insert_rowid]
        
      _saveParameters       $cmd $save_id
      _saveSpectrumDefs     $cmd $save_id
      _saveGateDefinitions  $cmd $save_id
      _saveGateApplications $cmd $save_id
      _saveTreeVariables    $cmd $save_id
      if {$spectra} {
        _saveSpectraContents  $cmd $save_id  
      }
      
        
    }
    return $save_id
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
# @param cmd - data base command.
# @return list of dicts -- see above.
proc listConfigs cmd {
    set result [list]
    $cmd eval {SELECT * FROM save_sets ORDER BY id ASC} {
        lappend result [dict create id $id name $name time $timestamp]
        
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
# @param save-name - Name of the save set.
# @param restoreSpectra - default false.
# @throw - error if save-name is not a save set.
#
#  Probably other errors will be thrown if cmd is a command for  a non
#  configuration database.
#
proc restoreConfig {cmd savename {restoreSpectra 0}} {
    set saveId [_lookupSaveSet $cmd $savename]
    
    #  Now restore the bits and pieces:
    
    # Restore parameters:
    
    _restoreParamDefs        $cmd $saveId
    _restoreSpectrumDefs     $cmd $saveId
    _restoreGateDefs         $cmd $saveId
    _restoreGateApplications  $cmd $saveId
    _restoreTreeVariables    $cmd $saveId
    if {$restoreSpectra} {
        _restoreSpectrumContents $cmd $saveId    
    }
    

}
##
# saveSpectrum
#   Saves a single spectrum's contents into the specified save set.
#
# @param cmd      -- the database command.
# @param sname Save set name.
# @param specname -- the spectrum name.
# @note the spectrum definition must already be in the save set.
# @note if this spectrum has already had its contents saved in this saveset,
#       this overwrites the spectrum contents in that saveset.
# 
proc saveSpectrum {cmd sname specname} {
    $cmd transaction {
        set sid [_lookupSaveSet $cmd $sname];  #raises an error if no such set.
        set id [_getSpectrumId $cmd $sid $specname]
        _requireCompatible $cmd $id [lindex [spectrum -list $specname] 0]
        _deleteContentsIfExists $cmd  $id
        _saveSpectrumChans $cmd $id [scontents $specname]
    }
}
##
# restoreSpectrum
#   Restore the contents of a single spectrum from a save set in the database.
#
# @param cmd     - database command.
# @param sname   - Save set name.
# @param specname - Name of a saved spectrum to restore.
# @note - if there's an existing spectrum with that name it's silently deleted.
# @note - the spectrum restored is fully functional not a snapshot.
# @note - if there's no channel data associated with the spectrum you get an empty spectrum.
#
proc restoreSpectrum {cmd sname specname} {
    set sid [_lookupSaveSet $cmd $sname]
    set id  [_getSpectrumId $cmd $sid $specname]
 
    # If necessary define the spectrum, load it.
    
    _restoreSpectrumDefs $cmd $sid $id;     # Redefine the spectrum according to the database.
    _restoreSpectrumContents   $cmd $sid $id
    sbind $specname;
}

##
# saveAllSpectrumContents
#   save all spectra currently defined into a save set.
#
# @param database - command.
# @param  sname - saveset name
proc saveAllSpectrumContents {cmd sname} {
    $cmd transaction {
        set sid [_lookupSaveSet $cmd $sname]
        
        _saveSpectraContents  $cmd $sid
    }
 
}
##
#  restoreAllSpectrumContents
#     Restores the channels in all spectra with saved data in a
#
# @param cmd - database command.
# @param sname - save set name.
#
proc restoreAllSpectrumContents {cmd sname} {
    set sid [_lookupSaveSet $cmd $sname]
    
    _restoreSpectrumContents $cmd $sid 
}

##
# listSavedSpectra
#    Lists the set of spectra saved in a configuration
#
# @param cmd - database command.
# @param sname - configuration svae set name.
# @return list of spectrum names that have channels.
#
proc listSavedSpectra {cmd sname} {
    set sid [_lookupSaveSet $cmd $sname]
    
    set result [list]
    $cmd eval {
        SELECT DISTINCT name FROM spectrum_contents
        INNER JOIN spectrum_defs ON spectrum_defs.id = spectrum_contents.spectrum_id
        WHERE spectrum_defs.save_id = :sid
    } {
        lappend result $name
    }
    
    return $result
}

}