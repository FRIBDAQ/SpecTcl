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
package provide editparameter 1.0
package require snit
package require guiutilities

#  This class is a dialog that allows
#  the user to edit parameter properties.
#  Note that if the parameter being edited is not
#  a treeparameter a new one will be created 'free of charge'
#  when the edit has completed.
#
# Options:
#     -updatecommand     - Command executed after the user
#                          has requested the parameter properties
#                          to be updated (e.g. refresh a browser).
#     -parameter         - Name of the parameter to edit.
# Layout:
#    +--------------------------------------------------------------+
#    |   Name              Low    High   Bins   Units     Array?    |
#    | [              ]    [  ]   [  ]  [   ]   [      ]   ()       |
#    ----------------------------------------------------------------
#    |  <OK>    <Accept>   <Revert>   <Cancel>                      |
#    +--------------------------------------------------------------+
#
snit::widget editparameter {
    hulltype toplevel
    option -updatecommand {}
    option -parameter     {}
    option -createcommand    {}

    variable emptyString {<..........>}

    constructor args {
        # Make and layout the widgets first then
        # configure as the configuration is active over the widgets.
        #
        label $win.name     -text Name
        label $win.lolabel  -text Low
        label $win.hilabel  -text High
        label $win.binlabel -text Bins
        label $win.unitlbl  -text Units
        label $win.arlbl    -text {Array?}

        label  $win.parameter -text $emptyString
        entry $win.low       -width 5
        entry $win.high      -width 5
        entry $win.bins      -width 5
        entry $win.units
        checkbutton $win.parameterarray

        set action [frame $win.action -borderwidth 3 -relief groove]
        button $action.ok     -text Ok      -command "[mymethod acceptParameter]; [mymethod die]"
        button $action.accept -text Accept  -command [mymethod acceptParameter]
        button $action.revert -text Revert  -command [mymethod loadParameter]
        button $action.cancel -text Cancel  -command [mymethod die]
        button $action.help   -text Help    -command [list spectclGuiDisplayHelpTopic editParameter]

        # Layout the widgets:

        grid $win.name      $win.lolabel $win.hilabel $win.binlabel $win.unitlbl $win.arlbl
        grid $win.parameter $win.low     $win.high    $win.bins     $win.units   $win.parameterarray

        pack $action.ok $action.accept $action.revert $action.cancel $action.help -side left
        grid $action          -             -             -            -           - -sticky ew

        # Finally configure the widget.


        $self configurelist $args

    }
    # acceptParameter
    #      Accepts the parameter in the UI.
    #      This can only be done if all of the entries are full.
    #      If the parameter string is empty this is an error.
    #      If a treeparametr does not yet exist for this parameter one
    #      will be created.
    method acceptParameter {} {
        set name  [$win.parameter cget -text]
        set low   [$win.low get]
        set high  [$win.high get]
        set bins  [$win.bins get]
        set units [$win.units get];     # Units can be blank.

        if {($name == $emptyString)           ||
            ![string is double -strict $low]  ||
            ![string is double -strict $high] ||
            ![string is integer -strict $bins] } {
            tk_messageBox -icon error -title incomplete \
                -message {Please fill in the form completely}
        } else {
            if {[$self isArray]} {
                set path [split $name .]
                set index [lindex $path end]
                if {![scan $index %d idx] == 1} {
                    tk_messageBox -title notarray -icon error -message "$name is not an array"
                } else {
                    set base [join [lrange $path 0 end-1] .]
                    foreach parameter [parameter -list $base.*] {
                        set element [lindex $parameter 0]
                        $self setParameter $element $low $high $bins $units
                    }
                }
            } else {
                $self setParameter $name $low $high $bins $units
            }
        }
        set script $options(-updatecommand)
        if {$script != ""} {
            eval $script
        }
    }
    # isArray
    #    Determine if the array checkbutton is 'lit'
    # Returns:
    #    1   - User is aksing for an array set.
    #    0   - User is not asking for an array set.
    #
    method isArray {} {
       set name [$win.parameterarray cget -variable]
       global $name
       return [set $name]
    }
    # setParameter name low high bins units
    #      Set one parameter (factorization).
    # Parameters:
    #    name    - Name of the parameter
    #    low     - Low parameter limit
    #    high    - High parameter limit
    #    bins    - default bins
    #    units   - (possibly blank) units.
    #
    method setParameter {name low high bins units} {
        if {[llength [treeparameter -list $name]] == 0} {
            treeparameter -create $name $low $high $bins [list $units]
            set script $options(-createcommand)
            if {$script != ""} {
                eval $script $name
            }
        } else {
            treeparameter -setlimits $name $low $high
            treeparameter -setbins $name $bins
            treeparameter -setunit $name [list $units]
        }
    }
    # die
    #    Destroy ourself
    #
    method die             {} {
        destroy $win
    }
    # loadParameter
    #      Load the entries with the values for this parameter.
    #      There are 2 cases:
    #      - the parameter is a treeparameter:
    #          low, high, bins and units are loaded from the parameter description.
    #      - the parameter is not a tree parameter:
    #         low, high, bins and units are blanked.
    #
    method loadParameter   {} {
        set info [treeparameter -list [$win.parameter cget -text]]
        if {[llength $info] != 0} {
            set info [lindex $info 0]
            ::setEntry $win.low [lindex $info 2]
            ::setEntry $win.high [lindex $info 3]
            ::setEntry $win.bins [lindex $info 1]
            ::setEntry $win.units [lindex $info 5]
        } else {
            ::setEntry $win.low  {}
            ::setEntry $win.high {}
            ::setEntry $win.bins {}
            ::setEntry $win.units {}
        }
    }

    # -parameter setting
    #      React to the configure -parameter switch.
    #      the name is loaded into the parameter widget and our
    #      loadParameter does the rest.
    #
    onconfigure -parameter name {
        $win.parameter configure -text $name
        $self loadParameter
    }

}
