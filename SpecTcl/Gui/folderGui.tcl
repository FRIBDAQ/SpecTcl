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

#  This is the toplevel of the new SpecTcl GUI... loosely based on the
#  capabilities, but not the structure of the old treeparmaeter GUI
#  written by D. Bazin.

package provide SpecTclGui 1.0

lappend auto_path [file dirname [info script]]

package require snit
package require browser
package require pseudo
package require gate
package require spectrum
package require editvariable
package require editparameter
package require guiutilities
package require guihelp
package require guistate
package require applygate
package require datasource
package require filtercontrol
package require preferences

set LargestSource 50

namespace eval ::FolderGui {
    variable folderGuiParent     {}
    variable folderGuiStatusFrame {}
    variable folderGuiBrowser {}

}

proc ::FolderGui::updateBrowser {} {
    $::FolderGui::folderGuiBrowser update
}

#-------------- Global defaults for preferences ------------

# GuiPrefs::preferences is an array that contains
# the user preferences.  These can be modified
# by the user's ~/.SpecTcl file at startup time
# and by the user via Edit->Preferences...
#

namespace eval GuiPrefs {
    variable preferences
}

#   Default x/y channels in a 2-d.
#

set GuiPrefs::preferences(defaultXChannels)   1024
set GuiPrefs::preferences(defaultYChannels)   1024
set GuiPrefs::preferences(defaultDaqRoot)    /usr/opt/daq/current
set GuiPrefs::preferences(defaultBuffersize)  8192



#--------------- Utility functions -------------------------

# unGate spectrum 
#   Removes a gate that's applied to a spectrum.  The browser
#   is then updated to reflect the new state.
#
# Parameters:
#   spectrum - name of the spectrum to ungatge.

#
proc unGate spectrum {
    ungate $spectrum
    ::FolderGui::updateBrowser
    failsafeWrite

}

# displayScriptErrors filename errors
#   Displays the errors associated with sourcing a file.
#   The errors are displayed in a non-modal dialog named
#   .scriptErrors
#   Any existing dialog named .scriptErrors is destroyed first.
#
# Parameters:
#    filename   - Name of the file.
#    errors     - list of errors.  See incrementalSource
#                 for a description of this parameter..which is just
#                 the value returned from that function.
#

proc displayScriptErrors {filename errors} {
    destroy .scriptErrors

    set top [toplevel .scriptErrors]
    label  $top.filename -text "$filename had errors: "
    grid   $top.filename   -     -
    label  $top.lineHead -text "Line #"
    label  $top.commandHead  -text "Command"
    label  $top.errorHead    -text "Error"

    grid $top.lineHead $top.commandHead $top.errorHead

    set errNumber 0
    foreach error $errors {


	set line    [lindex $error 0]
	set cmd     [lindex $error 1]
	set message [lindex $error 2]

	label $top.line$errNumber  -text $line  \
	    -borderwidth 2 -relief groove -justify left
	label $top.cmd$errNumber   -text $cmd \
	    -borderwidth 2 -relief groove -justify left
	label $top.msg$errNumber   -text $message   \
	    -borderwidth 2 -relief groove -justify left

	grid $top.line$errNumber $top.cmd$errNumber $top.msg$errNumber \
	    -sticky nsew

	incr errNumber
    }

    button $top.dismiss -text Dismiss -command [list destroy .scriptErrors]
   grid   x   $top.dismiss     x


}
# getLine fd
#     Returns a full line from a file.  This proc deals with lines ending in 
#     \ as indicating a continuation line.
#
#
proc getLine fd {
    set line ""
    set lines 0

    while {![eof $fd]} {
	gets $fd fragment
	incr lines
	if {[regexp {\\$} $fragment]} {
	    append line [regsub {\\$} $fragment " "]
	} else {
	    append line $fragment
	    return [list $line $lines]
	}
    }
    return [list $line $lines]
}


# incrementalSource file
#    Executes a Tcl script a command at a time.
#    If an error occurs, it is caught and appended to a list
#    of error messages that is returned to the caller.
#    Each error message list entry is a 3 element list consisting
#    of:
#      Line number of start of error.
#      offending command
#      error message.
#
# Parameters:
#   filename   - The name of the file to source.
#                it is the caller's responsibility to ensure this is
#                a readable file.
#
proc incrementalSource filename {
    set fd [open $filename r]
    set lineNumber 1
    set errors [list]

    while {![eof $fd]} {

	# Asssemble a command from fragments:

	set firstLine $lineNumber
	set info        [getLine $fd]
	incr lineNumber [lindex $info 1]
	set  command    [lindex $info 0]

	while {![eof $fd] && ![info complete $command]} {
	    set info [getLine $fd]
	    set fragment [lindex $info 0]

	    append command $fragment "\n"
	    incr lineNumber [lindex $info 1]
	}
	# If we got here, the command is complete or we've
	# run out of file.


	if {[catch {eval $command} msg]} {
	    lappend errors [list $firstLine $command $msg]
	}
    }


    close $fd
    return $errors
}

#------------------- menu action procs -------------------------


# editPrefs
#   Edit the user preferences.
#   We also save the new preferences for the user.
proc editPrefs {} {
    preferences::editPrefs
    preferences::savePrefs
}


# sourceScriptReportingErrors
#   Prompt for and source tcl/tk script but report
#   all the errors by incrementally executing it...
#   The entire script is run, ignoring errors.
#   At the end, if errors have occured a dialog box
#   that shows for each error:
#     The line number at which the command started,
#     The offending command.
#     The error message
#  
proc sourceScriptReportingErrors {} {
    set file [tk_getOpenFile -defaultextension .tcl             \
                             -filetypes [list                   \
                                    [list "Tcl Scripts"  .tcl]  \
                                    [list "Tk Scripts"   .tk]   \
                                    [list "All files"    *]]    \
                             -title {Select File to Source}]
    if {[file readable $file]} {
	set errors [incrementalSource $file]
	if {$errors ne ""} {
	    displayScriptErrors $file $errors
	}
        $::FolderGui::folderGuiBrowser update;       # In case the script execution changes something.
        failsafeWrite
    }
}

#sourceScript
#    Prompt for a tcl/tk script to source into the program.
#
proc sourceScript {} {
    set file [tk_getOpenFile -defaultextension .tcl             \
                             -filetypes [list                   \
                                    [list "Tcl Scripts"  .tcl]  \
                                    [list "Tk Scripts"   .tk]   \
                                    [list "All files"    *]]    \
                             -title {Select File to Source}]
    if {[file readable $file]} {
        if {[catch {uplevel #0 source $file} msg]} {
            tk_messageBox -icon error -title {Error in sourced script} \
                          -message "An error occured running the script: $file : $msg"
        }
        $::FolderGui::folderGuiBrowser update;       # In case the script execution changes something.
        failsafeWrite
    }
}

#exitProgram
#    Exit program with prompt
#
proc exitProgram {} {
    set no [tk_dialog .exitconfirm {Confirm Exit} \
            {Are you sure you want to exit the SpecTcl GUI?  SpecTcl will continue to run} \
            questhead 1 Yes No]
    if {!$no} {
        destroy $::FolderGui::folderGuiBrowser
        destroy .topmenu
 
    }
    failsafeWrite
}

# addGate name type description
#      Add a gate.  Called by the gate dialog propmter
#      when the user has entered a gate.
# Parameters:
#   name        - New gate name.
#   type        - SpecTcl gate type.
#   description - Gate specific description in SpecTcl form.
proc addGate {name type description} {
    if {$name == ""  || $description == "" || $type == ""} {
        tk_messageBox -icon error \
                      -message {You have not completely specified the gate, correct this in the gate editor}
        error {incomplete gate}
    }
    if {[gate -list $name] != ""} {
        set no [tk_dialog .confirmoverwrite {Overwrite gate} \
                    "You are requesting that an existing gate named $name be ovewritten" \
                    warning 1 Ok Cancel]
        if {$no} {
            error {User does not want to ovewrite}
        }
    }
    gate $name $type $description
    $::FolderGui::folderGuiBrowser update
    failsafeWrite
}
# addGateDestroyWidget widget name type description
#     Add a new gate and then destroy the prompting dialog.
#     See addGate for more information.
#
proc addGateDestroyWidget {widget name type description} {
    addGate $name $type $description
    destroy $widget
}


# createGate
#    Invoked when the user wants to create a new gate.  There are
#    several gate types in SpecTcl, each one may have a different
#    user interface for creation.
#    - Slices: s, gs - single point gates that only differ in whether
#              they depend on one or more than one parameter.
#    - PointLists: b, c, gb, gc - again the g's allow more than two parameters
#              in the dependency list.. and don't care which are
#              which, while the others have exactly one x, and one y parameter.
#    - CompoundGates *, +, -  - only allows a single dependency others allow
#              any number of dependent gates.
#    - c2band - Special compound case where two contours on the same pair of
#               parameters are joined to create a contour.
#    - masks  - em, am, nm  - Bitmask gates.
# NOTE:
#    For slices and pointlist gates, we let the user know that these can
#    be more easily entered in the Displayer by clicking on spectra than
#    here, but still allow them to do it.
#
proc createGate {} {
    if {[winfo exists .gategui]} return;         # It's already up.
    gateGui .gategui -cancelcommand [list destroy .gategui]      \
                     -acceptcommand [list addGate]               \
                     -okcommand     [list addGateDestroyWidget .gategui]
}
# editGate path
#    Same as createGate, but preloads a gate into the editor.
# Parameter:
#    Path to the gate.
#
proc editGate path {
    set name [::pathToName $path]
    createGate
    .gategui loadGate $name

}

# createPseudoParameter
#    Invoked when the user wants to create a new pseudo parameter.
#    We create the pseudo parameter editor widget, and collect
#    from it:
#       The final parameter name.
#       The expression to evaluate
#       The treeparameter limit information.
#  If the parameter name corresponds to an existing parameter,
#  we warn.  If the pseudo already exists and the user
#  agreed to overwrite the definition, we delete it
#  The script we write returns the low limit unless the
#  all dependent parameters are valid.
#
proc createPseudoParameter {args} {
    set name {}
    set body {}
    set deps {}
    if {[llength $args] != 0} {
        set name $args
        set info [pseudo -list $name]
        if {[llength $info] != 0} {
            set info [lindex $info 0]
            set body [lindex $info 2]
            set deps [lindex $info 1]
        }
    }

    if {[winfo exists .pseudo]} {
        .pseudo configure -name $name
        .pseudo configure -body $body
        .pseudo configure -dependencies $deps
        set treeinfo [treeparameter -list $name]
        if {$treeinfo != ""} {
            set treeinfo [lindex $treeinfo 0]
            .pseudo configure -bins [lindex $treeinfo 1]
            .pseudo configure -low  [lindex $treeinfo 2]
            .pseudo configure -high [lindex $treeinfo 3]
            .pseudo configure -units [lindex $treeinfo 5]
        }
        return;      #Dialog already up.
    }
    #
    #   Post the dialog and get the results from it before
    #   destroying it.
    #
    pseudoDialog .pseudo

    .pseudo configure -name $name
    .pseudo configure -body $body
    .pseudo configure -dependencies $deps
    set treeinfo [treeparameter -list $name]
    if {$treeinfo != ""} {
        set treeinfo [lindex $treeinfo 0]
        .pseudo configure -bins [lindex $treeinfo 1]
        .pseudo configure -low  [lindex $treeinfo 2]
        .pseudo configure -high [lindex $treeinfo 3]
        .pseudo configure -units [lindex $treeinfo 5]
    }

    while {1} {
        .pseudo      modal

        # The user may have 'x-d' the dialog:

        if {![winfo exists .pseudo]} return

        # Pull info from the dialog.

        set name     [.pseudo cget -name]
        set depends  [.pseudo cget -dependencies]
        set low      [.pseudo cget -low]
        set high     [.pseudo cget -high]
        set bins     [.pseudo cget -bins]
        set units    [.pseudo cget -units]
        set body     [.pseudo cget -body]


        # If the name is blank that's a sign the user cancelled.

        if {$name != "" } {
            # Low high and bins must be valid:
            #
            if {![string is double -strict $low]    ||
                ![string is double -strict $high]   ||
                ![string is integer -strict $bins]} {
                tk_messageBox -icon error -title {Bad Values} \
                    -message {Low and High must be floating point values and bins must be an integer.
Please correct your input on the form}
                continue
            }
            # Require that all the dependencies exist as parameters
            # treeparameterhood is not required:

            catch {unset badparameters}
            foreach dependency $depends {
                if {[parameter -list $dependency] == ""} {
                lappend badparameters $dependency
                }
            }
            # Report an error if there are any bad parameters

            if {[info exists badparameters]} {
                tk_dialog .badparameters "Bad parameters" \
                    "You are attempting to create a pseudo with dependencies on the following nonexistent parameters: $badparameters" \
                    error 0 Dismiss
                continue
            }
            #  If the name is an existing parameter warn the user about that too:
            #
            if {[parameter -list $name] != ""} {
                set overwrite [tk_dialog .overwriteok "Overwite ok" \
                              "The parameter $name already exists do you want to replace this definition?" \
                               questhead 1 Yes No]
                if {$overwrite == 1} {
                    continue
                }
            }
            #  Create a new treeparameter or modify an existing one if it already exists.

            if {[treeparameter -list $name] == ""} {
                # New

                treeparameter -create $name $low $high $bins $units

            } else {
                # Existing

                treeparameter -setlimits $name $low $high
                treeparameter -setbins   $name $bins
                treeparameter -setunit  $name $units
            }

            # Create the pseudo script...deleting the old one if necessary.

            if {[pseudo -list $name] != ""} {
                pseudo -delete $name;
            }
            pseudo $name $depends $body

            # Refresh the browser to show the new parameter:

            $::FolderGui::folderGuiBrowser update
            failsafeWrite

            break
        } else {
            break
        }
    }
    $::FolderGui::folderGuiBrowser update
    destroy .pseudo

}

#  applyGate  name
#       Lets the user choose and apply a gate to a spectrum.
# Parameters:
#    name   - The name of the spectrum.
# NOTE:
#     The dialog created is modal.
#
proc applyGate name {
    set application [apply -list $name]
    set application [lindex $application 0]
    set gateinfo    [lindex $application 1]
    set gatename    [lindex $gateinfo    0]

    applyGateOnSpectrum .gateselection -spectrum $name
    if {$gatename != "-T-"} {
        .gateselection configure -initialgate $gatename
    }

    if {[winfo exists .gateselection]} {
        .gateselection modal
        set gname [.gateselection getGate]
        destroy .gateselection

        if {$gname != ""} {
            apply $gname $name
            failsafeWrite
        }
        $::FolderGui::folderGuiBrowser update
    }
}
# applyGateToSpectra gatename
#        Asks the user which set of spectra the gate should
#        be applied to.  Note the dialog is modal.
# Parameters:
#  gatename - Name of the gate.
#
proc applyGateToSpectra gatename {

    #  Build up the list of spectra this gate is already
    #  applied to:

    set applications [apply -list]
    set appliedTo  [list]
    foreach application $applications {
        set spectrum [lindex $application 0]
        set info     [lindex $application 1]
        set gate     [lindex $info 0]
        if {$gate == $gatename} {
            lappend appliedTo $spectrum
        }
    }
    applyGateToMultiple .gatedialog -gate $gatename -applications $appliedTo
    .gatedialog modal

    if {[winfo exists .gatedialog]} {
        set appliedTo [.gatedialog cget -applications]
        if {[llength $appliedTo] > 0} {
	    foreach spectrum $appliedTo {
		apply $gatename $spectrum
	    }
        }
        destroy .gatedialog
        $::FolderGui::folderGuiBrowser update
    }

}
# deleteGate name
#     Delete the named gate assuming the user confirms this.
# Parameters:
#   name - The name of the gate
#
proc deleteGate name {
    set answer [tk_messageBox -title {Confirm Delete} -icon question \
                      -message "Are you sure you want to delete the gate: $name?" \
                      -type okcancel]
    if {$answer == "ok"} {
        gate -delete $name
        failsafeWrite
        $::FolderGui::folderGuiBrowser update
    }
}
#---------------------------------- Menu creation procs ---------

# createSpectrumFolderContextMenu
#      Creates the context menu for the top level Spectra folder.
#      This menu looks like:
#        New...         (Create a new spectrum)
#        Refresh Tree   (Updates browser contents).
#        ----------------------------------
#        Clear all      (Clear counts in all spectra).
#
#
proc createSpectrumFolderContextMenu {} {
    set men [menu .spectrumfoldercontextmenu -tearoff 0]
    $men add command   -label New...         -command [list createSpectrum]
    $men add command   -label {Refresh Tree} -command [list $::FolderGui::folderGuiBrowser update]
    $men add separator
    $men add command   -label {Clear all} -command {clear -all}
    $men add separator
    $men add command   -label Help -command [list spectclGuiDisplayHelpTopic Spectra]
}
# createParameterFolderContextMenu
#      Creates the parameter context menu.
#      The parameter context menu contains the following:
#      New...       (Create a new ordinary treeparameter).
#      Pseudo...    (Create a 'simple' pseudo parameter from an expression).
#      ----------------------------------------------------------------------
#      Refresh Tree  (Refresh browser's contents.
#
proc createParameterFolderContextMenu {} {
    set men [menu .parameterfoldercontextmenu -tearoff 0]
    $men add command -label Pseudo... -command createPseudoParameter
    $men add separator
    $men add command -label {Refresh Tree} -command {$::FolderGui::folderGuiBrowser update}
    $men add separator
    $men add command -label Help -command [list spectclGuiDisplayHelpTopic Parameters]

}
# createGateFolderContextMenu
#      Creates the context menu associated with the top level
#      Gates folder.  This menu has the following elements:
#      New...            (Compose a new gate).
#      -----------------------------------------
#      Refresh Tree      (Update the contents of the browser).
#
proc createGateFolderContextMenu {} {
    set men [menu .gatefoldercontextmenu -tearoff 0]
    $men add command -label New... -command createGate
    $men add separator
    $men add command -label {Refresh Tree} -command {::FolderGui::updateBrowser}
    $men add separator
    $men add command -label {Help}         -command [list spectclGuiDisplayHelpTopic Gates]

}
# createVariableFolderContextMenu
#      Creates the context menu associated with the top level
#      Variables folder.  For now, this folder only allows you
#      to refresh the contents of the browser.
#
proc createVariableFolderContextMenu {} {
    set men [menu .variablefoldercontextmenu -tearoff 0]
    $men add command -label {Refresh Tree} -command {::FolderGui::updateBrowser}
    $men add separator
    $men add command -label Help           -command [list spectclGuiDisplayHelpTopic Variable]
}
# createSXpectrumContextMenu
#     Creates the context menu associated with a right click
#     over a spectrum element.
#
proc createSpectrumContextMenu {} {
    set men [menu .spectrumcontextmenu -tearoff 0]
    $men add command -label {Edit...}
    $men add command -label {Clear}
    $men add command -label {Gate...}
    $men add command -label {UnGate}
    $men add separator
    $men add command -label {Write...}
    $men add separator
    $men add command -label {Delete...}
    $men add separator
    $men add command -label {Help} -command [list spectclGuiDisplayHelpTopic Spectra]
}
# createParameterContextMenu
#        Create the context menu for parameters.  This is popped
#        up on a right click on a parameter.
#
proc createParameterContextMenu {} {
    set men [menu .parametercontextmenu -tearoff 0]
    $men add command -label Edit...
    $men add command -label Pseudo...
    $men add separator
    $men add command -label Help -command [list spectclGuiDisplayHelpTopic Parameters]

}
# createGateContextMenu
#       Creates the context menu for gates.  This is popped up
#       on a right click over a gate.
#
proc createGateContextMenu {} {
    set men [menu .gatecontextmenu -tearoff 0]
    $men add command -label Edit...
    $men add command -label {Apply To...}
    $men add command -label {Delete...}
    $men add command -label Help   -command [list spectclGuiDisplayHelpTopic Gates]

}
#--------------------- procs to popup context menus -----------
#
# spectrumFolderContextMenu x y
#      Called when the user wants to choose from the spectrum
#      context menu.  This menu is a popup menu that is posted
#      by a right click on the Spectra top level folder of the
#      browser.  If necessary, the menu is created.. then
#      popped up under the pointer.
# Parameters
#   x,y   - Coordinates of the pointer (where to post the menu).
#
proc spectrumFolderContextMenu {x y} {
    if {[winfo exists .spectrumfoldercontextmenu] == 0} {
        createSpectrumFolderContextMenu
    }
    tk_popup .spectrumfoldercontextmenu $x $y 0
}
# parameterFolerContextMenu x y
#     Called when the user is asking the context menu
#     associated with the Parameters top level folder to be
#     posted (via a right click on that menu).  If necessary,
#     the context menu is created.  The menu is popped up
#     under the pointer.
# Parameters:
#    x,y    - The pointer coordinates.
#
proc parameterFolderContextMenu {x y} {
    if {![winfo exists .parameterfoldercontextmenu]} {
        createParameterFolderContextMenu
    }
    tk_popup .parameterfoldercontextmenu $x $y 0
}
#
# gateFolderContextMenu x y
#      Called when the user is asking for the Gates top level
#      folder context menu to be posted.  If necessary, the menu
#      is created.  The menu is popped up under the pointer.
# Paramters:
#   x,y    - The pointer coordinates.
#
proc gateFolderContextMenu {x y} {
    if {![winfo exists .gatefoldercontextmenu]} {
        createGateFolderContextMenu
    }
    tk_popup .gatefoldercontextmenu $x $y 0
}
# variableFolderContextMenu x y
#     Called when the user is asking for the Variable top level
#     folder context menu to be posted.  If necessary, the menu
#     is created.  The menu is popped up  under the pointer.
#  Parameters:
#    x,y   - Pointer coordinates.
#
proc variableFolderContextMenu {x y} {
    if {![winfo exists .variablefoldercontextmenu]} {
        createVariableFolderContextMenu
    }
    tk_popup .variablefoldercontextmenu $x $y 0
}
# spectrumContextMenu path
#      Pops up the context menu associated with a specific
#      spectum.  If necessary, the menu is created.
#      The menu is popped up over the element that is right clicked.
#
# Parameters:
#    path -  Path to the element clicked on
#    x,y  -  Where to pop up the context menu.
#
proc spectrumContextMenu  {path x y} {
    if {![winfo exists .spectrumcontextmenu]} {
        createSpectrumContextMenu
    }
    # Configure the menu commands so that they will
    # operate on the correct spectrum.
    #  The menu has:
    #       edit...
    #       clear
    #       Apply Gate...
    #       ---------
    #       Write...
    #       ------------
    #       delete...
    #

    set name [pathToName $path]

    .spectrumcontextmenu entryconfigure 0 -command [list editSpectrum   $path]
    .spectrumcontextmenu entryconfigure 1 -command [list clear $name]
    .spectrumcontextmenu entryconfigure 2 -command [list applyGate  $name]
    .spectrumcontextmenu entryconfigure 3 -command [list unGate     $name]

    .spectrumcontextmenu entryconfigure 5 -command [list writeSpectrum  $name]

    .spectrumcontextmenu entryconfigure 7 -command [list deleteSpectrum $name]

    tk_popup .spectrumcontextmenu $x $y
}
# parameterContextMenu path x y
#        Called in response to a right click on a parameter.
#        The parameter context menu is popped up at x/y and
#        configured to operate on the parameter described by the
#        path.
# Parameters:
#    path   - The path on which the right click occured.
#    x,y    - The coordinates at which the context menu should be poppped.
#
proc parameterContextMenu {path x y} {
    if {![winfo exists .parametercontextmenu]} {
        createParameterContextMenu
    }

    set name [pathToName $path]

    #  Configure the menu commands to operate on $name
    #      Edit...
    #      Pseudo...
    .parametercontextmenu entryconfigure 0 -command [list modifyParameter       $path]
    .parametercontextmenu entryconfigure 1 -command [list createPseudoParameter $name]

    #  Pop up the menu:

    tk_popup .parametercontextmenu $x $y

}
# gateContextMenu path x y
#       Called in response to a right click over a gate.
#       If necessary, the gate context menu is created.
#       The gate context menu is programmed to operate on
#       the gate represented by 'path'.
#       The gate context menu is popped up at x,y.
# Parameters:
#    path   - Browser path that represents the gate that was right clicked.
#    x,y    - Coordinates of the pointer (where the menu is popped up.
#
proc gateContextMenu {path x y} {
    if {![winfo exists .gatecontextmenu]} {
        createGateContextMenu
    }
    #
    set name [pathToName $path]

    #  Configure the menu commands to operate on $name.
    #      Edit...
    #      Apply To...
    #      Delete...
    #
    .gatecontextmenu entryconfigure 0  -command [list editGate $path]
    .gatecontextmenu entryconfigure 1  -command [list applyGateToSpectra $name]
    .gatecontextmenu entryconfigure 2  -command [list deleteGate $name]


    tk_popup .gatecontextmenu $x $y
}
# -------------------- Reactions to double clicks on elements.

# modifyVariable path
#     Called to allow the user to modify a variable.
#     A prompter for the variable value and units is
#     brought up.
# Parameters:
#     path   - Full path to the parameter in the brower.
proc modifyVariable path {
    if {![winfo exists .editvariable]} {
        editvariable .editvariable -updatecommand [list failsafeWrite; ::FolderGui::updateBrowser]
    }
    set name [pathToName $path]
    .editvariable configure -variable $name

}
# modifyParameter paths
#     Called to allow the user to modify a parameter.
#     A prompter for the parameter properties is created if
#     necessary and configured to edit the parameter.
#
# Parameters:
#    path - Full path to the parameter in the browser.
#
proc modifyParameter path {
    if {![winfo exists .editparameter]} {
        editparameter .editparameter -updatecommand "failsafeWrite; ::FolderGui::updateBrowser"
    }
    .editparameter configure -parameter [pathToName $path]
}

#
#   Determine how many bytes of display memory are in use:
#
proc spectrumUsage {} {
    set multiplier(long)  4
    set multiplier(word)  2
    set multiplier(byte)  1

    if {[catch {::spectrum -list} spectra]} {
	set spectra [list]
    }

    set usage 0

    foreach spectrum $spectra {
	
	# Figure out the channel count for the spectrum:

	set axes [lindex $spectrum 4]
	set dtype [lindex $spectrum 5]
	set stype [lindex $spectrum 2]

	set xChannels [lindex [lindex $axes 0] 2]
	if {[llength $axes] == 1} {
	    set channels $xChannels
	    if {$stype == "s"} {
		set paramcount [llength [lindex $spectrum 3]]
		set channels [expr $paramcount * $xChannels]
	    }
	} else {
	    set yChannels [lindex [lindex $axes 1] 2]
	    set channels [expr $xChannels * $yChannels]
	}

	# Multiply depending on the size of each channel:

	set bytes [expr $channels * $multiplier($dtype)]

	set usage [expr $usage + $bytes]
	
    }
    return $usage

}
# updateStatus nms
#      Maintains the status line in an updated state.
# Parameters:
#   nms   - Number of ms between updates.
#
set updateCount 0
global spectrumMemory
proc updateStatus nms {
    global RunTitle
    global RunNumber
    global RunState
    global BuffersAnalyzed
    global LastSequence
    global LargestSource
    global updateCount
    global spectrumMemory



    # It's always possible the user destroyed the window so conditionalize
    # the update on the window's existence.

    if {[winfo exists $::FolderGui::folderGuiStatusFrame.statusline1] 
	&& [winfo exists $::FolderGui::folderGuiStatusFrame.statusline2]} {

        after $nms [list updateStatus $nms];           # Reschedule.


	#  The spectrum memory part takes a bit of time to update, and only rarely changes
	# so only do that part every now and then.

	if {$updateCount == 0} {

	    set spectrumMemory [spectrumUsage]
	    set spectrumMemory [expr $spectrumMemory/(1024*1024)]
	}
	incr updateCount
	if {$updateCount >= 10} {
	    set updateCount 0
	}
	set outOf          ""
	if {[info globals DisplayMegabytes] ne ""} {
	    set outOf "/$::DisplayMegabytes"
	}

			    
        $::FolderGui::folderGuiStatusFrame.statusline1 configure -text \
            [format "Display memory: %s%s MB   Title %s Run Number: %s" $spectrumMemory $outOf $RunTitle $RunNumber]
        set source  [attach -list]
        if  {$LastSequence > 0} {
            set efficiency [expr 100.0*$BuffersAnalyzed/$LastSequence]
        } else {
            set efficiency 100
        }
        if {$RunState} {
            set state Active
        } else {
            set state Inactive
        }
	#
	#  For some data sources, the status line can get terribly long.
	#  for example attach -pipe cat {list of 100 file}
	#  If the status line is more than LargestSource chars long, we replace the middle
	#  characters with ...
	#

	set sourceLen [string length $source]
	if {$sourceLen > $LargestSource} {
	    set remove [expr $sourceLen - $LargestSource]
	    set midpoint [expr $sourceLen/2]
	    set start  [expr $midpoint - $remove/2]
	    set stop   [expr $midpoint + $remove/2]
	    set source [string replace $source $start $stop ...]
	}

	# format statusline 2.



        $::FolderGui::folderGuiStatusFrame.statusline2 configure -text \
            [format "Data Source: %s (%s) %d Buffers Analyzed %.2f%% efficient" $source $state $BuffersAnalyzed $efficiency]
        }

}
#-----------------------------------------------------------------
#
#   Entry point.  We create the browser in a new toplevel
#  widget.
#
# @param top - Existing top level widget to which menu is attached.  If
#              left empty, a new top level is created.
# @param parent - If top is present, this must be defined as well and is the
#                 parent widget into which the initial browser is built.
#              

proc ::FolderGui::startFolderGui {{top {}} {parent {}}} {
    if {$top eq ""} {
	if {[winfo exists .gui]} {
	    return;		# Don't start twice.
	}
	toplevel .gui -menu .topmenu
	set parent .gui
	set ::FolderGui::folderGuiStatusFrame $parent
    } else {
	if {$parent eq ""} {
	    error "folderGUI - providing a top level requires a parent as well"
	} 
	if {[winfo exists $parent.b]} {
	    return;		# Don't start twice.
	}
	$top configure -menu .topmenu
	set ::FolderGui::folderGuiStatusFrame [ttk::frame $top.spectclstatus]
    }

    set ::FolderGui::folderGuiParent  $parent
    set ::FolderGui::folderGuiBrowser $parent.b


    menu .topmenu
    menu .topmenu.filemenu -tearoff 0
    .topmenu.filemenu add command -label Save...    -command saveState
    .topmenu.filemenu add command -label {Save TreeVariables...} -command writeVariables
    .topmenu.filemenu add command -label {Save Contents of Spectra...} -command saveSeveralSpectra
    .topmenu.filemenu add separator
    .topmenu.filemenu add command -label Load... -command "restoreState; ::FolderGui::updateBrowser; FolderGui::updateBrowser"
    .topmenu.filemenu add command -label {Read Spectrum File...}      -command readSpectrumFile
    .topmenu.filemenu add command -label {Source Tcl Script...} \
                                                -command sourceScript
#    .topmenu.filemenu add separator
#    .topmenu.filemenu add command -label Exit... -command exitProgram

    menu .topmenu.edit -tearoff 0
    .topmenu.edit add command -label Preferences.. -command editPrefs

    menu .topmenu.help -tearoff 0
    .topmenu.help add command -label Topics...  -command spectclGuiHelpTopics
    .topmenu.help add command -label SpecTcl... -command spectclProgramHelp
    .topmenu.help add command -label About.. -command aboutMeDialog

    menu .topmenu.source -tearoff 0
    .topmenu.source add command -label {Online (spectrodaq)...} -command attachOnline
    .topmenu.source add command -label {File...}                -command attachFile
    .topmenu.source add command -label {Pipe...}                -command attachPipe
    .topmenu.source add separator
    .topmenu.source add command -label {List of runs...}        -command attachRunList
    .topmenu.source add command -label {Filter File...}         -command attachFilter
    .topmenu.source add separator
    .topmenu.source add command -label {Detach}                 -command detach


    menu .topmenu.filter -tearoff 0
    .topmenu.filter add command -label {Filter Wizard...}       -command createFilter
    .topmenu.filter add command -label {Filter Enables...}      -command enableFilters
    .topmenu.filter add separator
    .topmenu.filter add command -label {Read Filter file...}    -command attachFilter

    menu .topmenu.spectra -tearoff 0
    .topmenu.spectra add command -label {Save Contents of Spectra...} -command saveSeveralSpectra
    .topmenu.spectra add command -label {Read Spectrum File...}       -command readSpectrumFile
    .topmenu.spectra add separator
    .topmenu.spectra add command -label {Clear All}       -command [list clear -all]
    .topmenu.spectra add separator
    .topmenu.spectra add command -label {Create...}       -command createSpectrum
    .topmenu.spectra add command -label {Delete...}       -command selectAndDeleteSpectra
    .topmenu.spectra add separator
    .topmenu.spectra add command -label {Apply Gate...}   -command "selectAndApplyGate; ::FolderGui::updateBrowser"

    menu .topmenu.gate -tearoff 0
    .topmenu.gate add command -label Create...        -command "createGate; ::FolderGui::updateBrowser"
    .topmenu.gate add command -label Apply...         -command "selectAndApplyGate; ::FolderGui::updateBrowser"
    .topmenu.gate add separator
    .topmenu.gate add command -label Delete...        -command "selectAndDeleteGates; ::FolderGui::updateBrowser"

    .topmenu add cascade -label File -menu .topmenu.filemenu
    .topmenu add cascade -label Edit -menu .topmenu.edit
    .topmenu add cascade -label Help -menu .topmenu.help
    .topmenu add cascade -label {Data Source} -menu .topmenu.source
    .topmenu add cascade -label {Filters}     -menu .topmenu.filter
    .topmenu add cascade -label {Spectra}     -menu .topmenu.spectra
    .topmenu add cascade -label {Gate}        -menu .topmenu.gate




    browser $parent.b -spectrumfoldercommand   spectrumFolderContextMenu  \
                   -parameterfoldercommand  parameterFolderContextMenu \
                   -gatefoldercommand       gateFolderContextMenu      \
                   -variablefoldercommand   variableFolderContextMenu  \
                   -variablescript          modifyVariable             \
                   -parameterscript         modifyParameter            \
                   -gatescript              editGate                    \
                   -spectrumscript          editSpectrum                \
                   -spectrumrightclick      spectrumContextMenu        \
                   -parameterrightclick     parameterContextMenu        \
                   -gaterightclick          gateContextMenu
    pack $parent.b -fill both -expand 1


    
    ttk::label $::FolderGui::folderGuiStatusFrame.statusline1 \
	-justify left -text {Title:  N/A     Run Number: N/A}
    ttk::label $::FolderGui::folderGuiStatusFrame.statusline2 \
	-justify left -text {Data Source:  Test (inactive)   0 buffers analyzed  100% efficient}

    pack $::FolderGui::folderGuiStatusFrame.statusline1 -fill x -expand 0 -anchor w
    pack $::FolderGui::folderGuiStatusFrame.statusline2 -fill x -expand 0 -anchor w
	
    updateStatus 1000
    set ::SpecTclIODwellMax 100

    preferences::readPrefs
}
