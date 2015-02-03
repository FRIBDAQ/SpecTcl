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
package provide pseudo 1.0
package require snit

# This package provides a modal dialog that prompts for pseudo parameter
# expressions.  The user can edit a Tcl evaluated pseudo command.
# The caller can then fetch the individual pieces of this pseudo
# from the dialog and use them to create the pseudo.
#
#    The dialog looks like:
#
#      +-----------------------------------------------------------+
#      |   Pseudo:     [                      ]  Units: [      ]   |
#      |   Depends on  [                      ]                    |
#      |   Low:   [       ]    High:   [      ]  Bins: [       ]   |
#      |                                                           |
#      |    +------------------------------------------------+-+   |
#      |    |                                                |^|   |
#      |    |                                                | |   |
#      |                  ...                                      |
#      |    |                                                |V|   |
#      |    +------------------------------------------------+-+   |
#      +-----------------------------------------------------------+
#      |    {Ok}    {Cancel}                                       |
#      +-----------------------------------------------------------+
#
#    [    ]   - Is an entry
#    The big box is a text widget and the narrow lane to the right a scrollbar
#    The { } buttons.
#
#
snit::widget pseudoDialog {
    hulltype toplevel

    variable hidden {}

    option -name          {}
    option -units         {}
    option -dependencies  {}
    option -low           {}
    option -high          {}
    option -bins          {}
    option -body          {}
    option -okcommand     {}
    option -cancelcommand {}

    constructor {args} {


        # We are organied in three frames. The top frame has all of the
        # lables and entries.
        # The middle frame the scrolling text widget.
        # The bottom frame the action buttons.

        set entries [frame $win.topcommand]
        set text    [frame $win.bottomcommand]
        set action  [frame $win.action -relief groove -borderwidth 3]

        # Create the text widget and it's scrollbars.

        text $text.body -width 72 -height 24 -xscrollcommand "$text.scrollx set" \
                                             -yscrollcommand "$text.scrolly set" \
                                             -wrap none
        scrollbar $text.scrollx -orient horizontal -command "$text.body xview"
        scrollbar $text.scrolly -orient vertical   -command "$text.body yview"

        #  Set up the labels and entries:

        label $entries.plabel -text {Pseudo: }
        entry $entries.pseudo -width 32
        label $entries.ulabel -text {Units: }
        entry $entries.units  -width 10

        label $entries.deplabel -text {Depends on : }
        entry $entries.depends  -width 45

        label $entries.lowlabel -text {Low: }
        entry $entries.low      -width 5
        label $entries.hilabel  -text {High: }
        entry $entries.high     -width 5
        label $entries.binlabel -text {Bins: }
        entry $entries.bins     -width 5
        pack  $entries




        # The buttons:

        button $action.ok     -text Ok     -command [mymethod onOk]
        button $action.cancel -text Cancel -command [mymethod onCancel]
        button $action.help   -text Help   -command [list spectclGuiDisplayHelpTopic editPseudo]

        #  Layout the dialog:

        grid $entries.plabel -row 0 -column 0 -sticky w
        grid $entries.pseudo -row 0 -column 1 -columnspan 3 -sticky w
        grid $entries.ulabel -row 0 -column 4
        grid $entries.units  -row 0 -column 5

        grid $entries.deplabel -row 1 -column 0 -sticky w
        grid $entries.depends  -row 1 -column 1 -columnspan 5 -sticky w

        grid $entries.lowlabel $entries.low \
             $entries.hilabel $entries.high \
             $entries.binlabel $entries.bins -row 2 -sticky w
        pack $entries -side top -expand 1 -fill x

        #  Layout the text widget and its scrollbars:

        grid $text.body $text.scrolly -sticky news
        grid $text.scrollx            -sticky news
        grid rowconfigure    $text 0   -weight 1
        grid columnconfigure $text 0   -weight 1
        pack $text -side top -expand 1 -fill x

        # Layout the action area.

        pack $action.ok $action.cancel $action.help -side left
        pack $action -side top -expand 1 -fill x

        $self configurelist $args

    }
    #-------------- Callbacks ------------------

    #  mymethod onOk
    #     Called in response to the Ok button being clicked.
    #     If the user has established an -okcommand script,
    #     this script is called with all occurences of
    #     %W replaced with the translation of $win.
    method onOk {} {
        $self dispatch -okcommand
        $self endModal
    }

    #  Cancel clicked:

    method onCancel {} {
        $self configure -name {}
        $self dispatch -cancelcommand
        $self endModal
    }


    #----------------- Option set processors.

    # onconfigure -name text
    #    Sets the parameter name entry to the text supplied.
    #
    onconfigure -name {text} {
        ::setEntry $win.topcommand.pseudo $text
    }
    # onconfigure -units value
    #    Sets the units entry to the text supplied.
    #
    onconfigure -units {value} {
        ::setEntry $win.topcommand.units $value
    }
    # onconfigure -dependencies value
    #    Sets the dependencies entry to the text supplied,
    #
    onconfigure -dependencies {value} {
        ::setEntry $win.topcommand.depends $value
    }
    # onconfigure -low value
    #     Sets the low limit value of the parameter to the
    #     text supplied.
    #
    onconfigure -low  {value} {
        ::setEntry $win.topcommand.low $value
    }
    # onconfigure -high value
    #    Sets the high limit value of the parameter to the
    #    text supplied.
    #
    onconfigure -high {value} {
        ::setEntry $win.topcommand.high $value
    }
    # onconfigure -bins value
    #    Sets the number of bins in the parameter to the
    #    supplied value.
    #
    onconfigure -bins {value} {
        ::setEntry $win.topcommand.bins $value
    }
    # onconfigure -body text
    #     Set the body of the pseudo to the text provided.
    #
    onconfigure -body {text} {
        $win.bottomcommand.body delete 0.0 end
        $win.bottomcommand.body insert end $text
    }

    #----------------- Option get processors.

    oncget -name  {
        return [$win.topcommand.pseudo get]
    }
    oncget -units {
        return [$win.topcommand.units get]
    }
    oncget -dependencies {
        return [$win.topcommand.depends get]
    }
    oncget -low {
        return [$win.topcommand.low get]
    }
    oncget -high {
        return [$win.topcommand.high get]
    }
    oncget -bins {
        return [$win.topcommand.bins get]
    }
    oncget -body {
        return [$win.bottomcommand.body get 0.0 end]
    }

    #----------------Widget commands

    # modal
    #    Turns the dialog into a modal one by
    #    creating the hidden window, giving
    #    $win focus, and grab and
    #    waiting for hidden to be destroyed.
    #
    method modal {} {
        set hidden [frame $win.hidden]

        focus $win
        tkwait window $hidden
    }

    #--------------- Utilities
    #
    method dispatch option {
        set script $options($option)
         if {$script != ""} {
            regsub -all %W $script $win script
            eval $script
        }
    }
    # endModal
    #     If the hidden widget exists, it is destroyed to end the
    #     modal wait:
    #
    method endModal {} {
        if {$hidden != ""} {
            destroy $hidden
        }
    }
}


