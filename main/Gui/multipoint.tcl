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

package provide multipointeditor 1.0
package require snit
package require browser
package require guiutilities

#  This package (reluctantly) supports editing 'multipoint' gates.
#  reluctantly because these are much easier to click in via the
#  displayer.
#   In any event, options:
#     -maxparams    - If non zero, the max number of parameters
#                     the user can choose (gamma gates e.g. are
#                     unlimited, but others max at 2).
#     -minpoints   - The minimum number of points the user must
#                    input (defaults to 2).
#
#   The editor appearance is (modulo some labels):
#
#   +--------------------------------------------------------------------+
#   |  +---------------------------------+    +-------------------+-+    |
#   |  |  parameter browser              |    |   parameter list  | |    |
#   |  |                                 |    +-------------------+-+    |
#   |  |                                 |    +-------------------+-+    |
#   |  |                                 |    | point list        | |    |
#   |  |                                 |    +-------------------+-+    |
#   |  |                                 |    Next point: [          ]   |
#   |  +---------------------------------+    {Accept} {Delete last}     |
#   +--------------------------------------------------------------------+
#

snit::widget multipointEditor  {
    option -maxparams   0
    option -minpoints   2

    constructor args {
        $self configurelist $args

        label $win.warning -text {NOTE: You can click this sort of gate in from the displayer too} -foreground red
        # Since the parameter list widget must be
        # defined in order to allow us to filter the
        # parameters, we create the rhs widgets first:
        #
        label     $win.parlabel   -text {Parameters}
        listbox   $win.parameters  -yscrollcommand [list $win.parscroll set]
        scrollbar $win.parscroll   -command        [list $win.parameters yview] -orient vertical

        label     $win.ptlabel    -text {X-Y Points}
        listbox   $win.points     -yscrollcommand [list $win.ptscroll set]
        scrollbar $win.ptscroll   -command        [list $win.points yview] -orient vertical

        label     $win.nxtlabel   -text {Next Point (x y): }
        entry     $win.nextpoint

        button    $win.accept     -text {Accept Point}  -command [mymethod acceptPoint]
        button    $win.delete     -text {Delete last}   -command [mymethod deleteLastPoint]

        #  Now the browser...

        browser   $win.browser    -restrict {parameters} -showcolumns [list low high bins units]       \
                                  -filterparameters  [mymethod parameterFilter] \
                                  -parameterscript   [mymethod selectParameter]

        # Lay out the widgets.
        grid $win.warning    -               -
        grid $win.browser $win.parlabel
        grid    ^         $win.parameters  -sticky nse
        grid  $win.parscroll -row 2 -column 2 -sticky nsw
        grid    ^         $win.ptlabel
        grid    ^         $win.points     -sticky nse
        grid  $win.ptscroll -row 4 -column 2 -sticky nsw
        grid    ^         $win.nxtlabel   $win.nextpoint
        grid    ^         $win.accept     $win.delete

        #  Event bindings.

        bind $win.points <Double-1>            [mymethod deleteNearestPoint %x %y]
        bind $win.parameters <Double-1>        [mymethod deleteNearestParameter %x %y]
        bind $win.nextpoint <KeyPress-Return>  [mymethod acceptPoint]

    }
    # reinit
    #    Initialize the editor to its default (empty state).
    method reinit {} {
        $win.points delete 0 end
        $win.parameters delete 0 end
        $win.nextpoint delete 0 end

        $win.browser update
    }
    # load name
    #      Load the editor with the gate specified by the name
    # Parameters
    #   name - The name of the gate to load.
    #
    method load name {
        $self reinit

        set info [gate -list $name]
        set info [lindex $info 0]
        set type [lindex $info 2]
        set descr [lindex $info 3]

        if {($type == "b") || ($type == "c")} {
            set parameters [lindex $descr 0]
            set points     [lrange $descr 1 end]
        } else {
            set points [lindex $descr 0]
            set parameters [lindex $descr 1]
        }
        foreach point $points {
            $self addPoint [lindex $point 0] [lindex $point 1]
        }
        foreach param $parameters {
            $win.parameters insert end $param
        }
        $win.browser update
    }
    # getDescription
    #      Retrieve a gate description from the current state of
    #      the editor.   The gate description returned is empty if
    #      - There are fewer than -minpoints points.
    #      - There are fewer than 2 parameters.
    #      - Any of the points is not a valid pair of floating point numbers.
    #      In addition the description list may be ordered differently depending
    #      on the value of -maxparams:
    #      - -maxparams == 2 implies a contour or a band which is described by
    #                        a 3 element list containig the x/y parameter,
    #                        and a list of points.
    #      - -maxparams != 2 implies a gamma contour of gamma band which is
    #                        described by a two element list containing the list
    #                        of points and list of parameters.
    #
    method getDescription {} {
        #  Retrieve the information from the gui elements.

        set points     [$win.points get 0 end]
        set parameters [$win.parameters get 0 end]

        # Ensure we have enough points and parametrs.

        if {([llength $points] < $options(-minpoints)) ||
            ([llength $parameters] < 2) } {
            return [list]
        }
        # Convert the the points from (x y) to x y pairs
        # if any don't parse correctly then return empty list

        foreach point $points {
            if {[scan $point "(%f, %f)" x y] != 2} {
                return [list]
            }
            lappend gatePoints [list $x $y]
        }

        #  Now return the gate in the appropriate format:

        if {$options(-maxparams) == 2} {
            set retval [list [lindex $parameters 0] \
                         [lindex $parameters 1] $gatePoints]
        } else {
            set retval [list $gatePoints $parameters]
        }
        return $retval

    }
    # getHelpTopic
    #     Return the help topic for this gate editor.
    #
    method getHelpTopic {} {
        return 2dGate
    }
    # acceptPoint
    #     Accept the user's point to the end of the point list.
    #     The user's point is in the nextpoint widget, and must
    #     be in the form x y where both are valid floats.
    #     The point is placed in the listbox as a coordinate
    #     pair of the form (%f, %f).
    #     If all goes well, the nextpoint widget is blanked.
    method acceptPoint {} {
        set pointText [$win.nextpoint get]

        if {[scan $pointText "%f %f" x y] != 2} {
            tk_messageBox -title "Bad Point" -icon error \
                -message {Invalid point: Must be two floating point numbers separated by whitespace}
        } else {
            $self addPoint $x $y
        }
    }
    # addPoint x y
    #    Add a point given the x/y coordinates.
    # Parameters:
    #   x,y   - The coordinates.
    #
    method addPoint {x y} {
        set pointText [format "(%f, %f)" $x $y]
        $win.points insert end $pointText
        $win.nextpoint delete 0 end
    }
    # deleteLastPoint
    #    Removes the last point from the point list widget.
    #
    method deleteLastPoint {} {
        $win.points delete end
    }
    # deleteNearestPoint x y
    #      Deletes the point from the point list closest to x y.
    #
    method deleteNearestPoint {x y} {
        $win.points delete @$x,$y
    }
    # deleteNearestParameter x y
    #   Remove the parameter nearest the
    #   point x y.  Forces a browser update to adjust the set
    #   of parameters shown.
    method deleteNearestParameter {x y} {
        set index [$win.parameters index @$x,$y]
        set param [$win.parameters get $index]
        $win.browser addNewParameter $param
        $win.parameters delete $index
        #$win.browser update
    }
    # parameterFilter desc
    #      Parameter filter script called by the browser
    #      to determine if each parameter deserves to be displayed.
    #      To be brief, we display the parameter only if it is not in
    #      the parameters list box.
    # Parameters:
    #    desc   - SpecTcl parameter description.
    # Returns
    #    1     - Display the parameter.
    #    0     - Don't display the parameter.
    #
    method parameterFilter {desc} {
        set name [lindex $desc 0]
        set used [$win.parameters get 0 end]

        if {[lsearch -exact $used $name] != -1} {
            return 0
        } else {
            return 1
        }
    }
    # selectParameter args
    #     Adds the selected parameter to the used parameter list.
    #     If the number of parameters is > -maxparams, the least
    #     recently accepted parameter is removed from the list.
    #     The parameter list is updated to give the filter a chance
    #     to adjust the list.
    #
    method selectParameter {name} {
        if {$name == ""} return
        set name [::pathToName $name]

        $win.parameters insert end $name
        $win.browser deleteElement parameter $name

        if {($options(-maxparams) != 0) &&
            ([$win.parameters index end] > $options(-maxparams))} {
            set oldParam [$win.parameters get 0]
            $win.parameters delete 0
            $win.browser addNewParameter $oldParam
        }
        # $win.browser update
    }

}
