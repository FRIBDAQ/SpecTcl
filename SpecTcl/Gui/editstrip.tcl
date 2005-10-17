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

package provide editstrip 1.0
package require snit
package require browser
package require guiutilities

#  Provides a megawidget to edit a stripchart spectrum.
#  Strip chart spectra chart the sum of the value of one
#  parameter as a function of the other.  The x axis parameter
#  is called. the 'time parameter' and the yaxis parameter is
#  called the 'value parameter'
#
#   The widget layout is:
#
#   +----------------------------------------------------------+
#   | +-----------------------------+     Time axis: <name>    |
#   | | Parameter and gate browser  |     lo high bins units   |
#   | |                             |    [ ] [  ] [  ] xxxx    |
#   | |                             |                          |
#   | |                             |     Value axis: <name>   |
#   | |                             |                          |
#   | |                             |                          |
#   | +-----------------------------+                          |
#   +----------------------------------------------------------+

snit::widget editstrip {
    option -browser

    variable emptyString {<..........>}
    variable currentAxis time
    variable axisFrames

    constructor args {
        $self configurelist $args

        set browser $options(-browser)
        set myframe [frame $win.myframe -relief groove -borderwidth 3]

        # Layout the axis definition UI elements:

        foreach axis {time value} {
            set frame [frame $myframe.$axis]
            set axisFrames($axis) $frame

            label $frame.axislabel -text "$axis axis:"
            label $frame.parameter -text $emptyString
            grid $frame.axislabel $frame.parameter     -                    -
            if {$axis == "time"} {
                label $frame.lolabel   -text Low
                label $frame.hilabel   -text High
                label $frame.binslabel -text Bins
                label $frame.unitlabel -text Units

                entry $frame.low    -width 5
                entry $frame.high   -width 5
                entry $frame.bins   -width 5
                label $frame.units  -text {}
                grid $frame.lolabel   $frame.hilabel   $frame.binslabel $frame.unitlabel
                grid $frame.low       $frame.high      $frame.bins      $frame.units

            }



            pack $frame -side top -fill x -expand 1

            # some bindings to let the user choose axis and dispose of parameters:

            bind $frame.axislabel <Button-1> [mymethod setCurrentAxis $axis]
            bind $frame.parameter <Button-1> [mymethod setCurrentAxis $axis]
            bind $frame.parameter <Double-1> [mymethod emptyParameter $axis]

        }
        pack $browser $myframe -side left -fill y -expand 1

        # set the browser up as needed by us:

        $browser configure -parameterscript  [mymethod selectParameter]       \
                           -filterparameters [mymethod filterParameters]


        # set the current axis highlighted.

        $self highlightFrames

    }
    #               ---- methods that inquire about editor state ---

    method reinit {} {
        foreach axis {time value} {
            $self emptyParameter $axis
        }
        ::setEntry $win.myframe.time.low {}
        ::setEntry $win.myframe.time.high {}
        ::setEntry $win.myframe.time.bins {}

        $self setCurrentAxis time
    }
    #  load name
    #       Loads the described spectrum into the editor.
    #       It is the caller's responsibility to ensure that
    #       the spectrum is a stripchart spectrum.
    # Parameters:
    #   name    - Name of the spectrum.
    #
    method load name {
        $self reinit

        set info   [spectrum -list $name]
        set info   [lindex $info 0]
        set parameters [lindex $info 3]
        set axes       [lindex $info 4]
        set axis       [lindex $axes 0]

        set time  [lindex $parameters 0]
        set value [lindex $parameters 1]
        set low   [lindex $axis 0]
        set high  [lindex $axis 1]
        set bins  [lindex $axis 2]
        set units {}

        set paraminfo [treeparameter -list $time]
        if {[llength $paraminfo] != 0} {
            set paraminfo [lindex $paraminfo 0]
            set units     [lindex $paraminfo 5]
        }
        set frame $win.myframe
        $win.myframe.time.parameter configure -text $time
        $win.myframe.value.parameter configure -text $value

        $self setTimeAxisInfo $low $high $bins $units


        $options(-browser) update
    }
    # getParameters
    #    Note the difference in parameters case between this and
    #    the next one.  This version returns only a 0 or 2 element
    #    list as expected by the client callers.
    #
    method getParameters {} {
        set result [$self getparameters]
        if {[llength $result] != 2} {
            return [list]
        } else {
            return $result
        }
    }
    # getHelpTopic
    #      Returns the name of the help topic for this editor.
    #
    method getHelpTopic {} {
        return stripchart
    }
    # getparameters:
    #     Returns the set of parameters the user has chosen.
    #     This version returns returns a 0,1, or 2 element list.
    #
    #
    method getparameters {} {
        set result [list]
        foreach axis {time value} {
            set frame $axisFrames($axis)
            set name [$frame.parameter cget -text]
            if {$name != $emptyString} {
                lappend result $name
            }
        }
        return $result
    }
    # getAxes
    #    Return the axis description list.
    #    this is an all or nothing, returning an empty list if
    #    any of the axis definition parameters are incorrect
    #
    method getAxes {} {
        set frame $axisFrames(time)
        set low    [$frame.low get]
        set high   [$frame.high get]
        set bins   [$frame.bins get]
        if {![string is double -strict $low] || ![string is double -strict $high] ||
            ![string is integer -strict $bins]} {
            return [list]
        }
        return [list [list $low $high $bins]]
    }

    #               ---- Internal methods -----

    # flipAxes
    #     Selects the next axis in order from the
    #     current axis
    #
    method flipAxes {} {
        if {$currentAxis == "time"} {
            set nextAxis value
        } else {
            set nextAxis time
        }
        $self setCurrentAxis $nextAxis
    }
    # selectParameter path
    #     Selects a new parameter and loads it into the current axis.
    #     the axis is then flipped to the next one.  If the parameter
    #     as low/high/bins suggestions, they are loaded as well.
    # Parameters:
    #    path   - Full path to the parameter entry (e.g. Parameter.name...
    #
    method selectParameter path {
        set name [::pathToName $path]
        set frame $axisFrames($currentAxis)
        $frame.parameter configure -text $name
        if {$currentAxis == "time"} {

            set info [treeparameter -list $name]
            if {[llength $info] != 0} {
                set info [lindex $info 0]
                set bins [lindex $info 1]
                set low  [lindex $info 2]
                set high [lindex $info 3]
                set units [lindex $info 5]

                $self setTimeAxisInfo $low $high $bins $units
            }
        }
        $self flipAxes
        $options(-browser) update
    }
    # setTimeAxisInfo low high bins units
    #      Sets the time axis gui parameters
    # Parameters:
    #   low, high   - Axis range.
    #   bins        - Bins on the time axis.
    #   units       - Units of the time axis.
    #
    method setTimeAxisInfo {low high bins units} {
        set frame $axisFrames(time)
        ::setEntry $frame.low  $low
        ::setEntry $frame.high $high
        ::setEntry $frame.bins $bins
        $frame.units configure -text $units

    }
    # filterParameters
    #       Determines which parameters will be allowed
    #       to appear in the browser.   This is used to
    #       remove parameters the user has already selected
    #       for an axis on the assumption that the user doesn't
    #       want to have the sma parmeter in use multiple times
    #       on a spectrum.
    # Parameters:
    #   description - The SpecTcl parameter description.
    #
    method filterParameters description {
        set parameters [$self getparameters]
        set name       [lindex $description 0]

        if {[lsearch -exact $parameters $name] == -1} {
            return 1
        } else {
            return 0
        }
    }
    # setCurrentAxis
    #    Select which axis will get the next parameter.
    # Parameters:
    #   axis   -  the axis (time or value).
    #
    method setCurrentAxis axis {
        set currentAxis $axis
        $self highlightFrames
    }
    # emptyParameter axis
    #      Remove a parameter from the selected axis.
    # Parameters:
    #   axis   - the axis
    #
    method emptyParameter axis {
        $self setCurrentAxis $axis
        set frame $axisFrames($axis)
        $frame.parameter configure -text $emptyString
        $options(-browser) update
    }
    # highlightFrames
    #     Sets a highlight on the current frame.
    #
    method highlightFrames {} {
        foreach axis {time value} {
            set frame $axisFrames($axis)
            if {$axis == $currentAxis} {
                set bgcolor cyan
            } else {
                set bgcolor [$frame cget -background]
            }
            $frame.axislabel configure -background $bgcolor
            $frame.parameter configure -background $bgcolor
        }
    }


}
