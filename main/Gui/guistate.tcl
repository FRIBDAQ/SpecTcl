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


#
#   This variable is used to keep track of which
#   gates have been written in our recursive
#   gate write procedure.  See the comments before
#   writeGateDefinitions
#
namespace eval guistate {
    variable gatestatus
    variable pseudostatus
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
            set depname [lindex $dependency 0]
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
proc writeTreeParameters fd {
    puts $fd "\n#New Tree Parameters:\n"

    set newTreeParameters [treeparameter -listnew]
    foreach parameter $newTreeParameters {
        set info [treeparameter -list $parameter]
        set info [lindex $info 0]
        set bins [lindex $info 1]
        set low  [lindex $info 2]
        set high [lindex $info 3]
        set units [lindex $info 5]
        # The catch is to ensure that the script keeps running if the tree parameter
        # already exists.

        puts $fd "catch {treeparameter -create [list $parameter] $low $high $bins [list $units]}"
    }

    puts $fd "\n#Modified Tree Parameters:\n"

    foreach  info [treeparameter -list] {
        set name [lindex $info 0]
        set bins [lindex $info 1]
        set low  [lindex $info 2]
        set high [lindex $info 3]
        set units [lindex $info 5]

        # Only write the ones that are not new and have changed...
        if {([lsearch $newTreeParameters $name] == -1) && [treeparameter -check $name]} {
            puts $fd "treeparameter -setlimits [list $name] $low $high"
            puts $fd "treeparameter -setbins   [list $name] $bins"
            puts $fd "treeparameter -setunit   $name [list $units]\n"
        }
    }

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
#       Write out the tree variables that have been modified
#       since we started.  The tree variables themeselves keep
#       track of whether or not they've been modified.
# Parameters:
#   fd    - The file descriptor gotten from the [open] on the output file.
#
proc writeTreeVariables fd {
    puts $fd "\n# Tree variable definitions:\n"

    foreach variable [treevariable -list] {
        set name  [lindex $variable 0]
        set value [lindex $variable 1]
        set units [lindex $variable 2]
        if {[treevariable -check $name]} {
            puts $fd "treevariable -set [list $name] $value [list $units]"
        }
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

        puts $fd "catch {spectrum -delete [list $name]}"
        puts $fd "spectrum [list $name] $type [list $parameters] [list $axes]"

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
        if {$gatename != "-TRUE-"} {
            puts $fd "apply [list $gatename]  [list $spectrum]"
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

        puts $fd "catch {filter -delete [list $name]}"
        puts $fd [list filter [list $name] [list $gate] [list $parameters]]
        puts $fd [list filter -file [list $file] [list $name]]
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
    writeSpectrumDefinitions $fd
    writeGateDefinitions     $fd
    writeGateApplications    $fd
    writeFilters             $fd
}
# saveState
#     Save the state of the system.
#
proc saveState {} {
    set file [tk_getSaveFile -defaultextension .tcl                    \
                             -title {Save To...}                       \
                             -filetypes [list                          \
                                         [list {Tcl Scripts} .tcl]    \
                                         [list {Tk Scripts}  .tk]     \
                                         [list {All Files}    *]]]
    if {$file != ""} {
        if {[catch {open $file w} msg]} {
            tk_messageBox -icon error -title {Not Writable}  \
                -message "Unable to create $file because: $msg"
            return
        }
        set fd $msg
        writeAll $fd
        close $fd
    }
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
    sourceScript
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
