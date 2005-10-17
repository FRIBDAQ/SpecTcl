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
package provide edit2d 1.0
package require snit
package require browser
package require guiutilities

#  This package provides an editor for 2-d ordinary spectra.
#  Options:
#     -browser     - provides a browser widget that can be used
#                    to allow the user to graphically select stuff.
#
#

snit::widget edit2d {
    option -browser

    variable emptyString {<..........>}
    variable currentParameter x
    variable paramFrames

    constructor args {
        $self configurelist $args

        set browser $options(-browser)
        $browser configure -filterparameters [mymethod parameterFilter]  \
                           -parameterscript  [mymethod selectParameter]

        # Each parameter description consists of a frame containing
        # the parameter name, low/high/bins/units information
        #
        set myframe [frame $win.myframe]

        set xframe [$self createParameterDescription x]
        set yframe [$self createParameterDescription y]

        pack $xframe $yframe -side top -fill y -expand 1
        pack $browser $myframe -side left -expand 1 -fill y

        array set paramFrames [list x $xframe y $yframe]


        # Bind clicks to the parameter label and field to select
        # that as next.
        # Bind double clicks on the parameter itself to remove it
        # and select it.

        foreach axis [list x y] {
            set frame $paramFrames($axis)

            bind $frame.parlabel   <Button-1> [mymethod setNextAxis $axis]
            bind $frame.parameter  <Button-1> [mymethod setNextAxis $axis]
            bind $frame.parameter  <Double-1> [mymethod clearAxis   $axis]
        }
        # Highlight the 'next' parameter/label:

        $self highlightParameter
    }
    # reinit
    #    Setup the dialog to accept a new spectrum by resetting
    #    the axis to x and clearing the axes values.
    #
    method reinit {} {
        $self clearAxis   x
        $self clearAxis   y
        $self setNextAxis x
    }
    # load name
    #     Load the editor with the description of an existing spectrum.
    # Parameters:
    #  name - Name of the spectrum.  It is up to the user to ensure that
    #         the spectrum name is a 2-d spectrum (or rather a spectrum that
    #         makes sense to load into the 2-d spectrum editor.
    #
    method load name {
        set info       [spectrum -list $name]
        set info       [lindex $info 0]
        set parameters [lindex $info 3]
        set axes       [lindex $info 4]

        foreach parameter $parameters axis $axes axisname {x y} {
            set parinfo [treeparameter -list $parameter]
            set low    [lindex $axis 0]
            set high   [lindex $axis 1]
            set bins   [lindex $axis 2]
            set units  {}
            if {[llength $parinfo] != 0} {
                set parinfo [lindex $parinfo 0]
                set units   [lindex $parinfo 5]
            }

            $self loadParameterInfo $axisname $parameter $low $high $bins $units
        }

    }
    # getParameters
    #     Return the list of parameters that are currently selected
    #     by the user for the spectrum.
    #     this can be empty.
    #
    method getParameters {} {
        set parameters [list]
        foreach axis [list x y] {
            set frame $paramFrames($axis)
            set name [$frame.parameter cget -text]
            if {$name != $emptyString} {
                lappend parameters $name
            }
        }
        return $parameters
    }
    method getHelpTopic {} {
        return spectrum2d
    }
    # getAxes
    #     Return the axis definitions to the caller.
    #     these are returned as a 2 element list where
    #     each element is a three element {low hi bins}
    #     sublist.
    #        If any of the low/high elements are
    #        not valid, then a blank is returned
    #        indicating the user has to fill this stuff
    #        in correctly.
    #
    method getAxes {} {
        foreach axis [list x y] {
            set frame $paramFrames($axis)
            set low  [$frame.low get]
            set high [$frame.high get]
            set bins [$frame.bins get]

            # Validate the contents of this axis:

            if {![string is double -strict $low]    ||
                ![string is double -strict $high]   ||
                ![string is integer -strict $bins]} {
                return [list]
            }
            set aDescription [list $low $high $bins]
            lappend axes $aDescription
        }
        return $axes

    }
    # parameterFilter item
    #      Determines which parameters should appearn
    #      in the parameter folder of the  browser.
    #      for each item that this proc returns true,
    #      that parameter is permitted to appear.
    #      In our case, we refuse to allow parameters
    #      that have been selected for axes.
    # Parameters:
    #  item  - The parameter description of a candidate item.
    method parameterFilter {item} {
        set parameters [$self getParameters]
        set name [lindex $item 0]

        if {[lsearch -exact $parameters $name] == -1} {
            return 1
        } else {
            return 0
        }
    }
    # nextParameter
    #    Switches axis parameter from x <--> y
    #
    method nextParameter {} {
        if {$currentParameter == "x"} {
            set currentParameter y
        } else {
            set currentParameter x
        }
        $self highlightParameter
    }
    # setNextAxis axis
    #    Selects the next axis to get a parameter.
    # Parameters:
    #  axis - The axis (x or y).
    #
    method setNextAxis axis {
        set currentParameter $axis
        $self highlightParameter
    }
    # clearAxis axis
    #     Removes the parameter from the selected axis
    #     and selects it for to receive the next parameter.
    # Parameters:
    #   axis  - The axis to clear (x or y).
    #
    method clearAxis axis {

        set currentParameter $axis
        $self highlightParameter
        set frame $paramFrames($axis)
        $frame.parameter configure -text $emptyString
        ::setEntry $frame.low    {}
        ::setEntry $frame.high   {}
        ::setEntry $frame.bins   {}
        $frame.units configure -text {}

        $options(-browser) update
    }
    # highlightParameter
    #   Highlights the parameter into which the next parameter goes by
    #   setting its background color to cyan.
    #   The 'normal background color' is gotten from the lowlabel.
    method highlightParameter {} {
        foreach axis [list x y] {
            set frame $paramFrames($axis)
            if {$axis == $currentParameter} {
                $frame.parlabel  configure -background cyan
                $frame.parameter configure -background cyan
            } else {
                set normal [$frame.lowlabel cget -background]
                $frame.parlabel   configure -background $normal
                $frame.parameter  configure -background $normal
            }
        }

    }
    # createParameterDescription
    #    Create a parameter description widget set.
    #    This consists of a parameter name and label
    #    A labelled set of entries for low, high and bins.
    #    and a labelled label for the units.
    # Parameters
    #   which - x or y describing which parameter is being
    #           created.
    # NOTE:
    #  The description is created in a frame and laid out
    #  in the frame, so the frame only has to be positioned
    #  on the outer container.
    # Returns:
    #   Name of the frame widget in which the parameter description
    #   was laid out.
    method createParameterDescription which {
        set frame [frame $win.myframe.$which -relief groove -borderwidth 3]

        #  The parameter name:

        label $frame.parlabel   -text "$which Parameter"
        label $frame.parameter  -text $emptyString

        #  The labels for low, high, bins and units:

        label $frame.lowlabel  -text { Low }
        label $frame.hilabel   -text {High }
        label $frame.binlabel  -text {Bins }
        label $frame.unitlabel -text {Units}

        #  The entries and the units

        entry $frame.low  -width 5
        entry $frame.high -width 5
        entry $frame.bins -width 5
        label $frame.units -text {    }

        # now grid the elements into $frame:

        grid $frame.parlabel
        grid $frame.parameter
        grid $frame.lowlabel $frame.hilabel $frame.binlabel $frame.unitlabel
        grid $frame.low      $frame.high    $frame.bins     $frame.units

        return $frame

    }
    # selectParameter name
    #       Accept a parameter for the current axis.
    #       The axis is put into the parameter label field
    #       and the axes swapped.
    # Parameters:
    #   name - the full path to the item in the browser.
    method selectParameter name {
        set name [::pathToName $name]

        set info [treeparameter -list $name]
        if {[llength $info] != 0} {
            set info [lindex $info 0]
            set low  [lindex $info 2]
            set high [lindex $info 3]
            set bins [lindex $info 1]
            set units [lindex $info 5]

       } else {
            set low   {}
            set high  {}
            set bins  {}
            set units {}
        }

        $self loadParameterInfo $currentParameter $name \
                $low $high $bins $units
        $self nextParameter
        $options(-browser) update
    }
    # loadParameterInfo axis name low high bins units
    #     Loads a parameter descriptor.
    # Parameters:
    #  axis   - The axis to use (x or y).
    # name    - Parameter name.
    # low,high - Axis limits.
    # bins     - Bins on the axis.
    # units    - Axis units.
    #
    method loadParameterInfo {axis name low high bins units} {
        set frame $paramFrames($axis)
        ::setEntry $frame.low $low
        ::setEntry $frame.high $high
        ::setEntry $frame.bins $bins
        $frame.parameter configure -text $name
        $frame.units configure -text $units

    }
}
