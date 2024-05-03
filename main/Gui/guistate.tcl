#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321
#

package provide guistate 1.0
package require Tk
package require Iwidgets
package require snit
package require dbconfig


#  Snidget to prompt for a save file.
#  This is intended to deal with enhancement request 208:
#  That request asks that selective saves be allowed .. that is 
#  saves where one can determine which sorts of items are written to the
#  save file.
#
#  Layout of the widget is:
#    +-----------------------------------------------------+
#    | Select Categories to write to file:                 |
#    | [ ] Tree Parameters       [ ] Pseudo parameters     |
#    | [ ] Tree Variables        [ ] Spectrum Definitions  |
#    | [ ] Gate Definitions      [ ] Gate Applications     |
#    | [ ] Filter definitions                              |
#    +-----------------------------------------------------+
#    | Select output file:                                 |
#    | +--------------------------------------------+      |
#    | | IWidgets file chooser dialog               |      |
#    | | filter initially set to .tcl               |      |
#    | +--------------------------------------------+      |
#    +-----------------------------------------------------+
#    | [ Ok   ]    [ Cancel ]    [  Help ]                 |
#    +-----------------------------------------------------+
#
# Options:
#     Name          Type      What
#    -treeparams    bool      If set, tree parameter definitions get saved.
#    -pseudoparams  bool      If set, pseudo parameter definitions are saved.
#    -treevariables bool      If set the tree variable definitions get saved.
#    -spectrumdefs  bool      If set spectrum definitions get saved.
#    -gatedefs      bool      If set, gate definitions should be saved.
#    -gateapps      bool      If set, gate applications should be saved.
#    -filters       bool      If set, filers definitions will be saved.
#    -text          bool      If set save in text (Tcl) format.
#
#    -filename      string    The filename selected.
#    
#    -okcommand    script     Called from ok button click.
#    -cancelcommand script    Called from cancel button click.
#    -helpcommand  script     Called from help button click.
#
# Methods:
#   modal                      Makes the dialog modal until it is destroyed.
#
snit::widget saveDefPrompt {
    hulltype toplevel

    # The save options are all initially set.

    option   -treeparams    1
    option   -pseudoparams  1
    option   -treevariables 1
    option   -spectrumdefs  1
    option   -gatedefs      1
    option   -gateapps       1
    option   -filters       1
    option   -text          1

    option   -filename

    #  The scripts are all initially blank.

    option   -okcommand     {}
    option   -cancelcommand {}
    option   -helpcommand   {}


    variable hidden        {}

    constructor args {
	$self configurelist $args

	#   The widget will use 3 frames stacked vertically.
	#   That's in order to get us the bordering.

	set optframe  [frame $win.optframe   -borderwidth 3 -relief groove]
	set fileframe [frame $win.fileframe  -borderwidth 3 -relief groove]
	set action    [frame $win.action     -borderwidth 3 -relief groove]



	#  Set up the checkbuttons in the top frame (options).  We will
	#  grid them sticky west to ensure the boxes all line up.
	#
	label       $optframe.title -text {Select categories to write to file}
	checkbutton $optframe.tp \
	    -variable ${selfns}::options(-treeparams) \
	    -text {Tree Parameters}
	checkbutton $optframe.pseudo \
	    -variable ${selfns}::options(-pseudoparams) \
	    -text {Pseudo Parameters}
	checkbutton $optframe.tv \
	    -variable ${selfns}::options(-treevariables) \
	    -text {Tree Variables} 
	checkbutton $optframe.spectra \
	    -variable ${selfns}::options(-spectrumdefs) \
	    -text {Spectrum Definitions}
	checkbutton $optframe.gatedefs \
	    -variable ${selfns}::options(-gatedefs) \
	    -text {Gate Definitions}
	checkbutton $optframe.gateapps \
	    -variable ${selfns}::options(-gateapps) \
	    -text {Gate Applications}
	checkbutton $optframe.filter \
	    -variable ${selfns}::options(-filters) \
	    -text {Filter Definitions}
    checkbutton $optframe.text \
        -variable [myvar options(-text)] \
        -text {As text (Tcl) file} -command [mymethod _toggleFileType $optframe.text]

	grid $optframe.title    -columnspan 2      -sticky w
	grid $optframe.tp       $optframe.pseudo   -sticky w
	grid $optframe.tv       $optframe.spectra  -sticky w
	grid $optframe.gatedefs $optframe.gateapps -sticky w
	grid $optframe.filter   $optframe.text     -sticky w

	# The middle frame only has an Iwidgets file choice widget.
	# and a title.

	label $fileframe.title -text {Save to which file?}
	iwidgets::fileselectionbox $fileframe.fs   \
	    -directory .                           \
	    -mask     *.tcl
	grid $fileframe.title -sticky w
	grid $fileframe.fs    -sticky ewns

	# The bottom frame has the action buttons:

	button $action.ok      -text Ok     -command [mymethod onOk]
	button $action.cancel  -text Cancel -command [mymethod onCancel]
	button $action.help    -text Help   -command [mymethod onHelp]
	
	pack $action.ok $action.cancel $action.help -side left


	# Now arrange the frames:

	grid $optframe    -sticky ewn
	grid $fileframe   -sticky nsew
	grid $action      -sticky ews


    }
    # 'public' methods:


    # modal - Make the dialog application modal.  Modality persists
    #         until either x, ok or cancel are clicked.
    #
    method modal {} {
	if {$hidden eq "" } {;		# Modality does not nest.
	    set hidden [frame $win.hidden]
	    wm deiconify $win
	    focus $win
	    grab $win
	    tkwait window $hidden
	    grab release $win
	}
    }
    # Configuration management:


    # -filename:

    onconfigure -filename path {
	setEntry $win.fileframe.fs.selection $path
	set dir [file dirname $path]
	set filter [file join $dirname *.tcl]
	setEntry $win.fileframe.fs.filter $filter

    }

    oncget -filename {
	return [$win.fileframe.fs get]
    }

    #  Internal methods 

    #   The file type has toggled, set the file mask appropriately:

    method _toggleFileType {widget} {
        if { $options(-text) } {
            set mask *.tcl
        } else {
            set mask *.db
        }
        $win.fileframe.fs configure -mask $mask
        $win.fileframe.fs filter
    }
    # Ok click.. dispatch and destroy hidden if it exists.
    #
    method onOk {} {
	if {$hidden ne ""} {
	    destroy $hidden
	    set hidden ""
	}
	$self Dispatch -okcommand

    }
    # Cancel click.

    method onCancel {} {
	if {$hidden ne ""} {
	    destroy $hidden
	    set hidden ""
	}
	$self Dispatch -cancelcommand
    }
    # Help clicked - does not affect modality...

    method onHelp {} {
	$self Dispatch -helpcommand
    }

    #  Dispatch a user script:

    method Dispatch option {
	set script $options($option)
	if {$script ne ""} {
	    eval $script
	}
    }
}


#
#   This variable is used to keep track of which
#   gates have been written in our recursive
#   gate write procedure.  See the comments before
#   writeGateDefinitions
#
namespace eval guistate {
    variable gatestatus
    variable pseudostatus

    variable writeDeletes 1;	# If true commands are emitted to delete spectra.
    variable observers;         # List of scripts that are executed during saves.`
    array set observers [list];	#    Initialized to empty.
}

# getDependentGates description
#       Returns the set of gates the described gate depends on
# Parameters:
#   description  - The gate description given by gate -list.
#
proc getDependentGates description {
    set type [lindex $description 2]

    #  Only some gates can have dependencies:
    #  Note that c2band gets transformed to a contour.

    if {$type == "*"    ||  $type == "+" || $type == "-"} {
        return [lindex $description 3]
    }
    return [list]
}
# listInfoToWriteInfo type info
#        Takes the gate dependent information from a
#        gate -list and transforms it into the
#        gate information required for a gate creation.
#        In many cases this means doing nothing but I think
#        it's necessary to do some mapping for bands and contours.
# Parameters:
#     type   - the gate type.
#     info   - The gate type dependent information
#
proc listInfoToWriteInfo {type info} {
    if {($type == "b") || ($type == "c")} {
        set params [lindex $info 0]
        set points [lrange $info 1 end]
        return [list [lindex $params 0] [lindex $params 1] $points]
    } else {
        return $info
    }
}

# writeGate  fd description
#          Write a gate to the file.
#          If the gates dependencies are already
#          all written, we can write the gate.
#          othwerwise, we recurse, writing all
#          unwritten dependencies
#          The array ::guistate::gatestatus is used to
#          keep track of whether or not gates have been written.
# Parameters:
#    fd          - File descriptor [open] on the output file.
#    description - The SpecTcl gate description string.k
#
proc writeGate {fd description} {
    set name [lindex $description 0]
    if {!$::guistate::gatestatus($name)} {
        set dependencies [getDependentGates $description]
        foreach dependency $dependencies {
            set depname $dependency
            if {!$::guistate::gatestatus($depname)} {
                writeGate $fd [lindex [gate -list $dependency] 0]
            }
        }
        # Now all of our dependencies have been written so:

        set type [lindex $description 2]
        set info [lindex $description 3]
        set writeinfo [listInfoToWriteInfo $type $info]
        puts $fd "gate [list $name] $type [list $writeinfo]"

        # Mark the gate written:

        set ::guistate::gatestatus($name) 1
    }
}

# writeComments fd
#    Writes the opening comments to the savefile.
#    I'm also going to take a bit of time to immortalize
#    myself in the comments.
#
# Parameters:
#    fd   - result of [open] on the file to be written.
proc writeComments fd {
    set time [clock format [clock seconds]]
    puts $fd "#  SpecTclGUI save file created $time"
    puts $fd "#  SpecTclGui Version: [package version SpecTclGui]"
    puts $fd "#      Author: Ron Fox (fox@nscl.msu.edu)"
}
# writeTreeParameters fd
#    Writes the tree parameter that have been modified from the
#    initial settings to file. This includes tree parameters that we
#    have created.
#    The global newTreeParameters is a list of the tree paramters that
#    were created this session.  Each of these is written out with
#    a treeparameter -create command.
#    Tree parameters keep track of whether or not they have been modified;
#    those that have been modified, but are not in the newTreeParametrs list
#    will have their properties written.
#
# Parameters:
#     fd   - The return value of the [open] that opened the output file.
#
# @note - tree parameters only get saved if their definition has changed
#         since C++ constructed them.
#
proc writeTreeParameters fd {
    

    puts $fd "\n#Tree params:\n"

     foreach  info [treeparameter -list] {
        set name [lindex $info 0]
        
        #   Only save if the parameter changed:
        
        if {[treeparameter -check $name]} {
            set bins [lindex $info 1]
            set low  [lindex $info 2]
            set high [lindex $info 3]
            set units [lindex $info 5]
            
            # Could be new could be modified....
            
            puts $fd "catch {treeparameter -create [list $name] $low $high $bins [list $units]}"
            puts $fd "treeparameter -setlimits [list $name] $low $high"
            puts $fd "treeparameter -setbins   [list $name] $bins"
            puts $fd "treeparameter -setunit   $name [list $units]\n"
        }
     }

    return



}
# writePseudo fd description
#       Implements the recursive dependency analysis and ordering
#       of the pseudo write.  The algorithm is just to determine the
#       parameters the pseudo is dependent on.  If any of them are
#       pseudos and have not been written yet, invoke this proc on
#       that parameter as well.
# Parameters:
#    fd          - File handle open on the file we are writing to.
#    description - Description of the pseudo to write.
#
proc writePseudo {fd description} {
    # Extract the elements of the pseudo:

    set name  [lindex $description 0]
    set params [lindex $description 1]
    set body  [lindex $description 2]

    # We're done if this has already been written:

    if {$::guistate::pseudostatus($name)} return

    # Ensure our dependencies are already written:

    foreach parameter $params {
        if {[array names ::guistate::pseudostatus $parameter] != ""} {
            if {!$::guistate::pseudostatus($parameter)} {
                set dependency [lindex [pseudo -list $parameter] 0]
                writePseudo $fd $dependency
            }
        }
    }
    # now we can write ourself.

    puts $fd "catch {pseudo -delete [list $name]}"
    puts $fd "pseudo [list $name] [list $params] [list $body]\n"

    # and mark ourself written:

    set ::guistate::pseudostatus($name) 1
}
# writePseudoParameters fd
#       Write a pseudo parameter definition for each pseudo that exists.
#       prior to each pseudo parameter definition, we write out
#       a caught command to delete the pseudo in case we are replacing
#       one that already exists.
#       Note that we must be careful about the order in which pseudos
#       are written as it is possible for pseudos to depend on other
#       pseudos and this is a definition order dependent thing.
#
# Parameters:
#   fd     - Return value from the [open] that opened the output file.
#
proc writePseudoParameters fd {

    puts $fd "\n# Pseudo parameter definitions\n"

    array unset ::guistate::pseudostatus

    #  Stock the pseudo status array:

    foreach pseudo [pseudo -list] {
        set name [lindex $pseudo 0]
        set ::guistate::pseudostatus($name) 0
    }

    # Now do the recursive write thing.
    #
    foreach pseudo [pseudo -list] {
        writePseudo $fd $pseudo
    }

}
# writeTreeVariables fd
#       Write out the tree variables.
# Parameters:
#   fd    - The file descriptor gotten from the [open] on the output file.
#
proc writeTreeVariables fd {
    puts $fd "\n# Tree variable definitions:\n"

    foreach variable [treevariable -list] {
        set name  [lindex $variable 0]
        set value [lindex $variable 1]
        set units [lindex $variable 2]
	puts $fd "treevariable -set [list $name] $value [list $units]"
    }
}
# writeSpectrumDefinitions fd
#        Writes the definitions of all spectra.
#        In order to prevent errors when users accumulate multiple
#        save files with overlapping data, we destroy  a spectrum
#        inside a catch block prior to restoring it.  This ensures that
#        if the spectrum exists our creation will work... and is harmless
#        if the spectrum does not exist.
# Parameters:
#   fd   - File descriptor returned from [open] to create the output file.
#
proc writeSpectrumDefinitions fd {
    puts $fd "\n# Spectrum Definitions\n"
    foreach spectrum [spectrum -list] {
        set name       [lindex $spectrum 1]
        set type       [lindex $spectrum 2]
        set parameters [lindex $spectrum 3]
        set axes       [lindex $spectrum 4]
	set datatype       [lindex $spectrum 5]

	if {$::guistate::writeDeletes} {
	    puts $fd "catch {spectrum -delete [list $name]}"
	}
        puts $fd "spectrum [list $name] $type [list $parameters] [list $axes] $datatype"

    }
}
# writeGateDefinitions fd
#     Writes all the gates to file.  Since gates can depend on other
#     gates, this requires a bit of effort. We maintain an array
#     indexed by gate name.  The array elements are true if the gate
#     has been written to file and false otherwise.
#     Our algorithm is recursive;
#        for each gate
#           If a gate has not yet been written:
#              If a gate has no dependencies, write it and mark it written.
#               If a gate has dependencies
#                   Write all gates it depends on that have not been written yet
#                   and mark these gates written
#                   Write the gate
#  Parameters:
#     fd    - The file descriptor that was returned from the [open] that created
#             the output file.
#

proc writeGateDefinitions fd {

    puts $fd "\n# Gate definitions in reverse dependency order\n "

    array unset ::guistate::gatestatus

    #  Stock the gate status array:

    foreach gate [gate -list] {
        set name [lindex $gate 0]
        set ::guistate::gatestatus($name) 0
    }
    # Now write each gate using the recursive algorithm:

    foreach gate [gate -list] {
        writeGate $fd $gate
    }
}
# writeGateApplications fd
#        Write commands to apply gates to spectra as needed.
#
proc writeGateApplications fd {
    puts $fd "\n# Gate Applications: \n"
    foreach application [apply -list] {
        set spectrum [lindex $application 0]
        set gate     [lindex $application 1]
        set gatename [lindex $gate 0]
        # ungated spectra are actually gated on -TRUE-
        if {$gatename != "-TRUE-" && $gatename != "-Ungated-"} {
            puts $fd "applygate [list $gatename]  [list $spectrum]"
        }
    }
}
# writeFilters fd
#       Writes the set of filters ot an outputfile.
#       Note that all filters are written in the disabled state!!!
#
#
proc writeFilters fd {
    #

    puts $fd "\n#  filter definitions: ALL FILTERS ARE DISABLED!!!!!!!\n"

    foreach filter [filter -list] {
        set name [lindex $filter 0]
        set gate [lindex $filter 1]
        set file [lindex $filter 2]
        set parameters [lindex $filter 3]
	set format [lindex $filter 5]

        puts $fd "catch {filter -delete [list $name]}"
        puts $fd [list filter [list $name] [list $gate] $parameters]
        puts $fd [list filter -file [list $file] [list $name]]

	# The catch is in case the filter format comes from a plugin that is not
        # loaded in this instance.

	puts $fd "catch {[list filter -format [list $name] [list $format]]}"
        puts $fd ""
    }
}

# writeAll fd
#      Write the state of the application.  The following are written to
#     a file in the form of a Tcl Script:
#        treeparameter definitions that have changed.
#        All Pseudo parameter definitions
#        Treevariable definitions that have changed.
#        All spectrum definitions
#        Gate definitions
#        Gate Applications
#    The file is also commented with:
#       The save date/time
#       The version of the gui that is saving this.
#
# Parameters:
#    fd  - A file desciptor open on the file to write.
#
proc writeAll fd {
    writeComments            $fd
    writeTreeParameters      $fd
    writePseudoParameters    $fd
    writeTreeVariables       $fd
    writeGateDefinitions     $fd
    writeSpectrumDefinitions $fd
    writeGateApplications    $fd
    writeFilters             $fd

    #  Now execute the observers at the global level:

    foreach observerName [array names ::guistate::observers] {
	uplevel #0 $::guistate::observers($observerName) $fd
    }

}

#
#  Called when the save state widget has had ok clicked.
#  Figure out what to save and save it...
#
# widget is the widget that fired the ok.
#  
proc saveSelectedState widget {
    if {[$widget cget -text]} {
        set filename [$widget cget -filename]
        set fd [open $filename w]
        writeComments $fd

        if {[$widget cget -treeparams]} {
        writeTreeParameters $fd
        }
        if {[$widget cget -pseudoparams]} {
        writePseudoParameters $fd
        }
        if {[$widget cget -treevariables]} {
        writeTreeVariables $fd
        }
        if {[$widget cget -spectrumdefs]} {
        writeSpectrumDefinitions $fd
        }
        if {[$widget cget -gatedefs]} {
        writeGateDefinitions $fd
        }
        if {[$widget cget -gateapps]} {
        writeGateApplications $fd
        }
        if {[$widget cget -filters]} {
        writeFilters $fd
        }

        close $fd

    } else {
        #Sqlite save note this is not selective.

        set filename [$widget cget -filename]
        if { [file exists $filename] } {
            file delete $filename
        }
        dbconfig::makeSchema $filename
        set db [dbconfig::connect $filename]
        dbconfig::saveForRustogramer $db
        $db destroy



    }
}

# saveState
#     Save the state of the system.
#
proc saveState {} {
    saveDefPrompt .savedef \
	-okcommand     [list saveSelectedState .savedef] \
	-helpcommand   [list spectclGuiDisplayHelpTopic savestatedialog]
    .savedef modal 
    catch [list destroy .savedef];	# Shouldn't need this.



    return

    
}
#  failsafeWrite
#      Write the file [pdw]failsafe.tcl
#      containing the current state.
#
proc failsafeWrite {} {
    set name [file join [pwd] failsafe.tcl]
    set fd [open $name w]
    writeAll $fd
    close $fd

}
# restoreState
#    Restore the state file.. this is the same as sourcing it
#
proc restoreState {} {
    sourceScriptReportingErrors
    sbind -all;				# Ensure spectra are visible.
}
# writeVariables
#       Writes only the treevariables to file:
#
proc writeVariables {} {
    set file [tk_getSaveFile -defaultextension .tcl                  \
                             -title {Save to...}                     \
                             -filetypes [list                        \
                                         [list {Tcl Scripts} .tcl]   \
                                         [list {Tk Scripts}  .tk]    \
                                         [list {AllFiles}    *]]]
    if {$file != ""} {
        if {[catch {open $file w} msg]} {
            tk_messageBox -icon error -title {Not Writable} \
                          -message "Unable to create file $file because $msg"
            return
        }
        set fd $msg
        writeTreeVariables $fd
        close $fd
    }
}
#-----------------------------------------------------------------------
#
#  Observer management code:
#

##
# Add a save state observer.  These are run in arbitrary order 
# after the main state is saved.
# @param name   - Name of the observer (should be unique).
# @param script - Script to run.  The file descriptor to which the
#                 save is being done is  appended to the script.
#
proc addSaveObserver {name script} {
    set ::guistate::observers($name) $script
}
##
# Remove a save state observer. 
# @name - Name of the observer to remove.  It is a no-op to remove an
#         observer that does not exist (not an error).
#
proc removeSaveObserver name {
    if {[array names ::guistate::observers $name] eq $name} {
	array unset ::guistate::observers $name
    }
}
