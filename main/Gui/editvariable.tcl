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
package provide editvariable 1.0
package require snit
package require guiutilities
package require SpecTclGui

#    Supplies an editor for treevariables.
#    The form of the editor is quite simple:
#
#   +-------------------------------------------+
#   |  Name            Value  Units    array?   |
#   |  [          ]   [    ]  [   ]     ()      |
#   |  <Ok>   <Accept>  <Revert> <Cancel>       |
#   +-------------------------------------------+
#
#  Options:
#      -variable name - Switches to a new variable.
#

snit::widget editvariable {
    hulltype toplevel

    option -variable {}
    option -updatecommand  {}

    variable emptyString {<..........>}
    constructor args {
        #   Set up the widget first since configuration
        #   is active and requires the widgets to work..

        label   $win.name     -text  Name
        label   $win.vallabel -text  Value
        label   $win.unitlbl  -text  Units
        label   $win.arrlbl   -text  Array?

        label   $win.variable -text  $emptyString
        entry   $win.value    -width  5
        entry   $win.units
        checkbutton $win.array

        set action [frame   $win.action  -borderwidth 3 -relief groove]
        button $action.ok      -text  Ok        -command "[mymethod setVariable]; [mymethod die]"
        button $action.accept  -text  Accept    -command [mymethod setVariable]
        button $action.revert  -text  Revert    -command [mymethod reloadVariable]
        button $action.cancel  -text  Cancel    -command [mymethod die]

        #  Layout the widget:

        grid $win.name     $win.vallabel $win.unitlbl $win.arrlbl
        grid $win.variable $win.value    $win.units   $win.array

        pack $action.ok $action.accept $action.revert $action.cancel -side left
        grid $action          -              -         - -sticky ew


        #   Process the options.
        $self configurelist $args

    }
    # setVariable
    #     Update the treevariable from the widget.
    #
    method setVariable {} {
        set name $options(-variable)
        if {$name != $emptyString} {
            set value [$win.value get]
            set units [$win.units get]

            if {[$self setArray]} {
                set path [split $name .]
                set last [lindex $path end]
                if {[string is integer -strict $last]} {
                    set path [lrange $path 0 end-1]
                    set basename [join $path .]
                    set arraynames [treevariable -list $basename.*]
                    foreach element $arraynames {
                        set ename [lindex $element 0]
                        treevariable -set $ename $value $units
                    }
                } else {
                    tk_messageBox -icon info           \
                        -title {Not array}             \
                        -message "$name is not a treevariable array. Not setting anything"
                }
            } else {
                treevariable -set $name $value $units

            }
	    ::FolderGui::updateBrowser
        }
        set script $options(-updatecommand)
        if {$script != ""} {
            eval $script
        }
    }
    # reloadVariable
    #     Reload the widget from the treevariable.
    #
    method reloadVariable {} {
        set info [treevariable -list $options(-variable)]
        if {$info != ""} {
            set info  [lindex $info 0]
            set name  [lindex $info 0]
            set value [lindex $info 1]
            set units [lindex $info 2]

            $win.variable configure -text $name
            ::setEntry $win.value $value
            ::setEntry $win.units $units
        }
    }
    # die
    #    Destroy self.
    method die {} {
        destroy $win
    }
    # setArray
    #      Determine if the 'array' checkbox is set.
    # Returns
    #   0   - Array checkbox not set.
    #   1   - Array checkbox is set.
    #
    method setArray {} {
        set name [$win.array cget -variable]
        global $name
        return [set $name]
    }
    # -variable set handler.
    #     The variable is looked up in the treeparams.  If it exists,
    #     the widget is loaded with the variable value and units.
    #
    onconfigure -variable value {
        set options(-variable) $value
        $self reloadVariable

    }
}
