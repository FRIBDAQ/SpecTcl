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
package provide dbconfig 0.1
package require sqlite3

namespace eval dbconfig {
#----------------------------------------------------------
#  Private procs.

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
#
proc _restoreSpectrumDefs {cmd sid} {
    # Get the spectrum top level defs.  These will go into an array of
    # dicts indexed by the spectrum id
    
    array set topdefs [list];          # Array of top level dicts.
    set idlist [list]
    $cmd eval {
        SELECT * from spectrum_defs  WHERE save_id = :sid
        ORDER BY id ASC
    } {
        set value [dict create id $id name $name type $type datatype $datatype]
        set topdefs($id) $value
        lappend idlist $id
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
# _restoreSpectrumContents
#    Restores the spectrum contents for a save set.
#    The assumption is that the spectrum has been defined.
#
# @param cmd - database command.
# @param sid - save-set id.
#
proc _restoreSpectrumContents {cmd sid} {
    $cmd eval {
        SELECT name, xbin, ybin, value FROM spectrum_defs
        INNER JOIN spectrum_contents ON spectrum_defs.id = spectrum_id
        WHERE save_id= :sid
        ORDER by spectrum_defs.id, spectrum_contents.id ASC
    } {
        set cmd [list channel -set $name]
        if {$ybin ne ""} {
            lappend cmd [list $xbin $ybin]
        } else {
            lappend cmd $xbin
        }
        lappend cmd $value
        
        eval $cmd
    }
}

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

    
}
##
# Save a configuration.  Only one configuration of a given name can exist.
#
#  @param name - Name of the configuration.
#  @return int - The id of the save's root record.
#
proc saveConfig {cmd name} {
    set timestamp [clock seconds]
    
    $cmd transaction {
        $cmd eval {INSERT INTO save_sets (name, timestamp) VALUES(:name, :timestamp)}
        set save_id [$cmd last_insert_rowid]
        
      _saveParameters $cmd $save_id
      _saveSpectrumDefs $cmd $save_id
      _saveSpectraContents $cmd $save_id
        
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
    set saveId ""
    
    $cmd eval {
        SELECT id FROM save_sets WHERE name = :savename
    } {
        set saveId $id
    }
    if {$saveId eq ""} {
        error "There is no save set named $savename"
    }
    
    #  Now restore the bits and pieces:
    
    # Restore parameters:
    
    _restoreParamDefs $cmd $saveId
    _restoreSpectrumDefs $cmd $saveId
    _restoreSpectrumContents $cmd $saveId
}


}