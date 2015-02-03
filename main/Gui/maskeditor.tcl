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

package provide maskeditor 1.0
package require snit
package require browser
package require BWidget
package require guiutilities

#    Provides an editor for the three sorts of mask gates.
#    The user can edit the gate either by typing in a
#    number (into an entry), or by flipping bits via checkboxes.
#    The bits are represented as checkboxes.  Masks are assumed
#    to be 32 bits wide.
#
#
snit::widget maskGateEditor {
    variable emptyString {<.................>}

    constructor {args} {
        $self configurelist $args

        # To the right of the browser we'll have a parameter name/label:

        label $win.plabel     -text {Parameter: }
        label $win.parameter  -text $emptyString


        # Setup a browser for parameters only, no detail and a script
        # To process doubleclicks.

        browser $win.browser -detail 0 -restrict parameters  \
                             -parameterscript [mymethod selectParameter] \
                             -filterparameters [mymethod parameterFilter] \
                             -showcolumns [list low high bins units] 


        # Below the browser we have the bit picker and the mask entry.
        #
        #  Two additional checkboxes as well: {set all} {clear all}
        #

        frame  $win.allframe -borderwidth 3 -relief groove
        button $win.allframe.clear -text {Clear All} -command [mymethod setMask 0]
        button $win.allframe.set   -text {Set All}   -command [mymethod setMask 0xffffffff]
        pack $win.allframe.clear $win.allframe.set -side left

        # A labelframe for each nybble.

        LabelFrame $win.n8 -side top -text {31 - 28}    -relief groove -borderwidth 3
        LabelFrame $win.n7 -side top -text {27 - 24}    -relief groove -borderwidth 3
        LabelFrame $win.n6 -side top -text {23 - 20}    -relief groove -borderwidth 3
        LabelFrame $win.n5 -side top -text {19 - 16}    -relief groove -borderwidth 3
        LabelFrame $win.n4 -side top -text {15 - 12}    -relief groove -borderwidth 3
        LabelFrame $win.n3 -side top -text {11 -  8}    -relief groove -borderwidth 3
        LabelFrame $win.n2 -side top -text { 7 -  4}    -relief groove -borderwidth 3
        LabelFrame $win.n1 -side top -text { 3 -  0}    -relief groove -borderwidth 3

        set bitoffset 0
        foreach frame {n1 n2 n3 n4 n5 n6 n7 n8} {
            set frame [$win.$frame getframe]
            foreach bit {0 1 2 3} {
                set bno [expr {$bit+$bitoffset}]
                checkbutton $frame.bit$bno -relief flat -command [mymethod updateEntry]
                grid $frame.bit$bno -row 0 -column [expr {3-$bit}]
            }
            incr bitoffset 4
        }
        # Now a label and an entry that allows the user to type stuff in too:

        label $win.bitlabel -text {Bit Mask: }
        entry $win.bits     -width 10

        #  Now the basic geometry:

        grid $win.browser - - - - - - x x
        grid  ^  ^ ^ ^ ^ ^ ^          $win.plabel $win.parameter
        grid $win.allframe $win.n8 $win.n7 $win.n6 $win.n5 $win.n4 $win.n3 $win.n2 $win.n1
        grid $win.bitlabel $win.bits

        # Event bindings to deal with typing in the entry.

        bind $win.bits <FocusOut>        [mymethod updateCheckboxes]
        bind $win.bits <KeyPress-Return> [mymethod updateCheckboxes]
    }
    #  load name
    #      Load the editor with the information from an existing mask gate.
    # Parameters:
    #   name  - The name of the gate.
    #
    #
    method load name {
        set info [gate -list $name]
        set info [lindex $info 0]
        set descr [lindex $info 3]
        set param [lindex $descr 0]
        set mask  [lindex $descr 1]

        $win.parameter configure -text $param
        $self setMask $mask

        $win.browser update
    }

    #  reinit
    #      Reinitilalize the widget to allow a new gate to be defined.
    #
    method reinit  {} {
        $win.parameter configure -text $emptyString
        $self setMask 0
        $win.browser update
    }
    # getDescription
    #     Returns the gate description string associated with the
    #     current widget state.  this will be empty if the gate is
    #     not well specified.
    #
    method getDescription {} {
        set parameter [$win.parameter cget -text]
        set mask      [$win.bits      get]

	# Note for Tcl 8.4 on debain integers with the top bit set
	# has hex strings make string is integer false, so I need to
	# get wierd to check validity...incr only operates on good
	# integers so a catch of incr on the mask should fail if the
	#  mask is not a valid integer:
	#
	set amask $mask
#        if {($parameter == $emptyString) || ![string is integer -strict $mask]} {
	#        }  
	if {($parameter == $emptyString)  || [catch {incr amask}]} {
            return [list]
        } else {
            return [list $parameter $mask]
        }
    }
    # getHelpTopic
    #      Returns the help topic for this editor.
    #
    method getHelpTopic {} {
        return maskGate
    }
    # selectParameter args
    #     Invoked when a parameter is double-left-clicked.  The selected parameter
    #     set made into the gate parameter.  The parameter is then also removed
    #     from the list of parameters in the browser (after an update).
    # Parameters:
    #    args  - a set of arguments to the callback that are ignored.
    #
    method selectParameter {selection} {
        set name [::pathToName $selection]
        $win.parameter configure -text $name
        $win.browser update
    }
    # updateCheckboxes
    #       Read the entry value and update the checkbox values from it
    #       If the entry value is not an integer then we complain
    #       and leave stuff alone on the theory that the
    #       user will eventually set the value correctly.
    #
    method updateCheckboxes {} {
        set mask [$win.bits get]

        if {![string is integer -strict $mask]} {
            tk_messageBox -icon error -title {Invalid Mask} \
                          -message {The bitmask entered is not a valid integer, or out of range, please correct this}
        } else {
            set shift 0
            set bno   0
            foreach nybble {n1 n2 n3 n4 n5 n6 n7 n8} {
                set frame [$win.$nybble getframe]
                foreach bit {1 2 4 8} {
                    set widget $frame.bit$bno
                    set thisbit [expr {$bit << $shift}]
                    if {($thisbit & $mask) != 0} {
                        $widget select
                    } else {
                        $widget deselect
                    }
                    incr bno
                }
                incr shift 4
            }
        }

    }
    # updateEntry
    #     Uses the values of the checkboxes to update the value of the entry
    #
    method updateEntry {} {
        set mask 0
        set shift 0
        set bno  0
        foreach nybble {n1 n2 n3 n4 n5 n6 n7 n8} {
            set frame [$win.$nybble getframe]
            foreach bit {1 2 4 8} {
                set thisbit  [expr $bit << $shift]
                set widget $frame.bit$bno
                set varname [$widget cget -variable]
                global $varname
                if {[eval set [list $varname]]} {
                    set mask [expr $mask | $thisbit]
                }
                incr bno
            }
            incr shift 4
        }
        $win.bits delete 0 end
        $win.bits insert end [format 0x%08x $mask]
    }
    # parameterFilter param-def
    #      Returns true if the parameter in param-def should be included
    #      in the parameter list.  In practice, the parameter should only
    #      be excluded if it is the selected parameter.
    # Parameters:
    #    param-def   - The parameter definition string.
    method parameterFilter descr {
        set currentParam [$win.parameter cget -text]
        set name         [lindex $descr 0]

        if {$currentParam == $emptyString} {
            return 1
        }
        if {$currentParam == $name} {
            return 0
        }
        return 1
    }
    # setMask value
    #      Set the mask to a specific value.  This is done by setting the
    #      entry and then updating the checkbuttons.
    # Parameters:
    #  value  - the new value (e.g 0x1234)
    #
    method setMask value {
        $win.bits delete 0 end
        $win.bits insert end $value
        $self updateCheckboxes

    }

}
