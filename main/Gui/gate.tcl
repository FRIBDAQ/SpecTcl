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


package provide gate 1.0
package require snit
package require compoundgate
package require c2band
package require maskeditor
package require sliceditor
package require multipointeditor
package require guiutilities

# There's an assumption that there is only one gate gui
# up at a time with this (the gate type radio button variable).
#


#  This snit megawidget provides a gate generation dialog.  The dialog
#  can be modal or not.. up to the user.  The Gui consists of three
#  frames.  The top frame contains a gate type selector. and gate name.
#  The middle frame morphs as needed to the correct type dependent gate
#  creation gui.
#  The bottom frame has the usual Ok Cancel buttons.
#  The contents of the middle frame are typically another megawidget.
#
#  Due to the variability, no drawings are provided.
#
#  Options:
#     -okcommand
#     -acceptcommand
#     -cancelcommand
#
snit::widget gateGui {
    hulltype toplevel

    option -okcommand
    option -acceptcommand
    option -cancelcommand

    variable gateType         {none}
    variable gateEditorWidget
    variable gateTypeNames
    variable helpTopic genericGate
    constructor {args} {

        $self configurelist $args
        array set gateTypeNames [list * And b Band c Contour c2band {2 Bands to a Contour} \
                                            F False gb {Gamma Band} gc {Gamma Contoure}     \
                                            gs {Gamma Slice} - Not + Or s Slice T True      \
                                            em {Equals Mask} am {And Mask} nm {Not Mask}]

        set type     [frame $win.type];               # Select gate type.
        set action   [frame $win.action];             # Type dependent gate editor.
        set command  [frame $win.command];            # Dismissal buttons.

        # Setup the gate type selector.  The gate type
        # selector consists of a drop down menu with all the gate types.
        #

        menubutton $type.menu  -menu $type.menu.toplevel -text {Gate Type} \
                               -relief raised -borderwidth 4
        set typemenu [menu       $type.menu.toplevel -tearoff 0]
        $typemenu add command -label {And (*)}          -command [mymethod startCompoundEditor *]
        $typemenu add command -label {Or  (+)}          -command [mymethod startCompoundEditor +]
        $typemenu add command -label {Not (-)}          -command [mymethod startCompoundEditor -]
        $typemenu add command -label {2 Bands to Contour} -command  [mymethod startC2BandEditor]
        $typemenu add separator
        $typemenu add command -label {Equal mask}       -command [mymethod startMaskEditor em]
        $typemenu add command -label {And mask}         -command [mymethod startMaskEditor am]
        $typemenu add command -label {Not Mask}         -command [mymethod startMaskEditor nm]
        $typemenu add separator
        $typemenu add command -label slice              -command [mymethod startSliceEditor s]
        $typemenu add command -label {Gamma slice}      -command [mymethod startSliceEditor gs]
        $typemenu add separator
        $typemenu add command -label Band               -command [mymethod startMultipointEditor b]
        $typemenu add command -label Contour            -command [mymethod startMultipointEditor c]
        $typemenu add command -label {Gamma Band}       -command [mymethod startMultipointEditor gb]
        $typemenu add command -label {Gamma Contour}    -command [mymethod startMultipointEditor gc]

        label $type.namelbl -text {Gate Name: }
        entry $type.name  -width 32

        # layout the selector:

        pack $type.menu $type.namelbl $type.name  -side left
        pack $type -side top -expand 1 -fill x

        #  Put in a placeholder frame for the Gate gui editor.

        frame $win.editor ; # -width 256 -height 256
        set gateEditorWidget [label $win.editor.contents   \
                                    -text {Select gate type using the dropdown menu above}]
        pack $win.editor.contents    -anchor c
        pack $win.editor -side top   -expand 1 -fill both


        #  Setup the command frame contents and layout.
        #  then paste it into the window.

        button $command.ok     -text Ok     -command [mymethod onMakeGate -okcommand]     -state disable
        button $command.accept -text Accept -command [mymethod onMakeGate -acceptcommand] -state disable
        button $command.cancel -text Cancel -command [mymethod onCancel]
        button $command.help   -text Help   -command [mymethod onHelp]
        pack $command.ok $command.accept  $command.cancel $command.help -side left
        pack $command -side top -expand 1 -fill x

    }
    # onHelp
    #     Called when the help button is clicked.
    #     Displays the appropriate help topic.
    #
    method onHelp  {} {
        spectclGuiDisplayHelpTopic $helpTopic
    }
    # loadGate name
    #     Loads a specific gate into the gate editor.
    #     We load the correct type specific gate editor
    #     and configure it to contain the current gate.
    #     Note that if the gate is a T or an F gate, we
    #     only load the gate name as we don't have editors
    #     for T/F gates (nothing to edit).
    #
    method loadGate name {
        set info [gate -list $name]
        set info [lindex $info 0]
        set gtype [lindex $info 2]

        ::setEntry $win.type.name $name

        switch -exact -- $gtype {
            * -
            + -
            - {
                $self startCompoundEditor $gtype
                $win.editor.contents load $name
            }
            em -
            am -
            nm {
                $self startMaskEditor $gtype
                $win.editor.contents load $name
                }
            s -
            gs {
                $self startSliceEditor $gtype
                $win.editor.contents load $name

            }
            b -
            c -
            gb -
            gc {
                $self startMultipointEditor $gtype
                $win.editor.contents load $name
            }
            default {
            }

        }

    }
    #------------------ Internal methods and procs ------------------

    # onCancel
    #   Called whenthe cancel button is clicked.  IF there's a -cancelcommand
    #   it is called.
    #
    method onCancel {} {
        if {$options(-cancelcommand) != ""} {
            eval $options(-cancelcommand)
        }
    }
    # onMakeGate option
    #     Dispatches for gate creation.
    # Parameters:
    #  option   - The name of the option holding the user's script.
    # Call sequence:
    #    userscript gatename gatetype description
    #       The user must check for the possibility that any of these could
    #       be blank or invalid if the user has not finished filling in the editor.
    #
    method onMakeGate Option {
        set script $options($Option)
        if {$script != ""} {
            set name [$win.type.name get]
            set type $gateType
            catch {$win.editor.contents getDescription} description
            set error [catch {eval $script [list $name] [list $type] [list $description]} mes]
            if {!$error && [winfo exists $win]} {
                $win.editor.contents reinit
                $win.type.name delete 0 end
            }
        }
    }
    # startCompoundEditor type
    #     starts the compound gate editor.  The compound gate editor
    #     is used to edit any of the * + or - gates.   The - gate limits
    #     the number of subgates to exactly 1.
    #
    method startCompoundEditor gType {
        $self setGateType $gType

        if {[lsearch -exact {* + -} $gType] == -1} {
            tk_messageBox -icon error \
                          -message "Invalid compound gate type selected in startCompoundEditor $gType report this bug to fox@nscl.msu.edu please"
            return
        }
        destroy $gateEditorWidget

        set gateEditorWidget [compoundgate $win.editor.contents]

        # Not gates only allow at most 1 subgate.
        #
        if {$gType == "-"} {
            $gateEditorWidget configure -atmost 1
        }
        pack $gateEditorWidget -expand 1 -fill x
        set helpTopic [$gateEditorWidget getHelpTopic]

    }
    # startC2BandEditor
    #    Sart the Contour from 2 band editor.
    #
    method startC2BandEditor {} {
        $self setGateType c2band
        destroy $gateEditorWidget
        set gateEditorWidget [c2band $win.editor.contents]
        pack $gateEditorWidget -expand 1 -fill x
        set helpTopic [$gateEditorWidget getHelpTopic]

    }
    # startMaskEditor type
    #     Start the mask editor.  This editor creates the three
    #    types of bitmask gates.
    # Parameter:
    #     type  - the exact gate type requested.
    method  startMaskEditor {gType} {
        $self setGateType $gType

        destroy $gateEditorWidget

        set gateType $gType
        set gateEditorWidget [maskGateEditor $win.editor.contents]
        pack $gateEditorWidget -expand 1 -fill x
        set helpTopic [$gateEditorWidget getHelpTopic]
    }
    # startSliceEditor type
    #     Start the slice editor.  This editor.
    #     Valid slice gates are type s and gs.
    # Parameters:
    #    type   - Type of gate to create (e.g. s).
    #
    method startSliceEditor {gtype} {
        $self setGateType $gtype

        destroy $gateEditorWidget
        if {$gtype == "s"} {
            set gateEditorWidget [sliceEditor $win.editor.contents -maxparams 1]
        } else {
            set gateEditorWidget [sliceEditor $win.editor.contents]
        }
        pack $gateEditorWidget -expand 1 -fill x
        set helpTopic [$gateEditorWidget getHelpTopic]
    }
    # startMultipointEditor gtype
    #       Starts a gate editor that accepts a set of points.
    # Parameters:
    #    gtype   - The gate type:
    #              c  - Contour       at least 3 points, exactly 2 parameters.
    #              b  - Band          at least 2 points, exactly 2 parameters
    #              gc - Gamma Contour at least 3 points, at least 2 parameters.
    #              gb - Gamma Band    at least 2 points, at least 2 parameters.
    #
    method startMultipointEditor {gtype} {
        $self setGateType $gtype

        destroy $gateEditorWidget
        set gateEditorWidget [multipointEditor $win.editor.contents]

        switch -exact -- $gtype {
            c {
                $gateEditorWidget configure -minpoints 3 -maxparams 2
            }
            b {
                $gateEditorWidget configure -minpoints 2 -maxparams 2
            }
            gc {
                $gateEditorWidget configure -minpoints 3
            }
            gb {
                $gateEditorWidget configure -minpoints 2
            }
        }
        pack $gateEditorWidget -expand 1 -fill x
        set helpTopic [$gateEditorWidget getHelpTopic]
    }
    # setGateType g
    #       Sets the current gate type by setting the
    #       gate type menubutton appropriately and allowing ok/accept
    #       setting gateType.
    # Parameters:
    #   g   - the type of gate.
    method setGateType g {
        $win.type.menu configure -text $gateTypeNames($g)
        $win.command.ok configure -state normal
        $win.command.accept configure -state normal
        set gateType $g

    }

    #
    #
    # gateType
    #    Returns the current gate type.
    #
    method gateType {} {
        return $gateType
    }


}

# deleteMultipleGates
#         Allows you to select a set of gates for  deletion.
#         This is essentially a compound gate builder in a dialog box.
#
snit::widget deleteMultipleGates {
    hulltype toplevel

    option -okcommand
    option -cancelcommand

    variable hidden {}
    delegate method getDescription to multiSelector



    constructor args {
        install multiSelector using compoundgate $win.gates -dependentlabel {Gates To Delete}
        frame $win.command
        button $win.command.ok      -text Ok      -command [mymethod onOk]
        button $win.command.cancel  -text Cancel  -command [mymethod onCancel]
        button $win.command.help    -text Help    -command [list spectclGuiDisplayHelpTopic deleteMultipleGates]


        grid $win.gates
        pack $win.command.ok $win.command.cancel $win.command.help -side left
        grid $win.command -sticky w

        $self configurelist $args
    }
    # modal
    #      Switch over to modal dialog.
    #
    method modal {} {
        if {$hidden == ""} {
            set hidden [frame $win.hidden]
            wm deiconify $win
            focus $win
            grab $win
            tkwait window $hidden
        }
    }
    # onOk
    #     Called for the ok button click.
    method onOk {} {
        set script $options(-okcommand)
        if {$script != ""} {
            eval $script
        }
        if {$hidden != ""} {
            destroy $hidden
            set hidden ""
        }
    }
    # onCancel
    #    Called for the cancel button click.
    #
    method onCancel {} {
        $win.gates reinit

        set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }
        if {$hidden != ""} {
            destroy $hidden
            set hidden ""
        }
    }

}


# selectAndDeleteGates
#       Manages a dialog to select and delete a set of gates.
#
proc selectAndDeleteGates {} {
    deleteMultipleGates .deletegates
    .deletegates modal

    if {[winfo exists .deletegates]} {
        set gates [.deletegates getDescription]
        if {[llength $gates] > 0} {
            set answer [tk_messageBox -type yesno -icon question -title {Confirm}  \
                            -message "Are you sure you want to delete the gates: [join $gates {, }]"]
            if {$answer == "yes"} {
                foreach gate $gates {
                    gate -delete $gate
                }
            }
        }
        failsafeWrite
        destroy .deletegates

    }
}
