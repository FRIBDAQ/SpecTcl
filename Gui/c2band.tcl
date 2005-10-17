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

package provide c2band 1.0
package require snit
package require browser
package require guiutilities


#  This megawidget provides an editor for contours that are
#  made up of a pair of bands.  This editor requires that you select
#  x/y parameters before it exposes the appropriate set of gates
#  to you.  The appropriate set of gates is the set of bands that depend
#  on the two parameters selected.
#    This is all driven from a browser widget that has parameter and Gates
# Folders.  Double clicks are sensitive to which element type has been clicked on.
# The -filtergates option on the browser is used to filter the set of gates to the
# set that we want.
#    The editor widget appearance is:
#
#     +---------------------------------------------------------+
#     | +-----------------------------+   X parameter <param>   |
#     | |                             |   Y parameter <param>   |
#     | |                             |                         |
#     | |  Browser                    |   Band 1      <band>    |
#     | |                             |   Band 2      <band>    |
#     | +-----------------------------+                         |
#     +---------------------------------------------------------+
#

snit::widget c2band {
    variable parwidgets
    variable bandwidgets


    variable currentParameter
    variable currentBand
    variable emptyString {<..................>}


    constructor {args} {
        $self configurelist $args

        label $win.xparlabel -text {X parameter}
        label $win.yparlabel -text {Y parameter}
        label $win.xparameter -text $emptyString
        label $win.yparameter -text $emptyString


        label $win.b1label   -text {Band 1}
        label $win.b2label   -text {Band 2}
        label $win.b1        -text $emptyString
        label $win.b2        -text $emptyString


        browser $win.browser -detail 0 -restrict {parameters gates} -filtergates [mymethod filterGates] \
                             -parameterscript [mymethod selectParameter]                                \
                             -gatescript      [mymethod selectGate]

        grid $win.browser $win.xparlabel $win.xparameter
        grid      ^       $win.yparlabel $win.yparameter
        grid      ^       $win.b1label   $win.b1
        grid      ^       $win.b2label   $win.b2

        # The arrays below hold the current set of parameters/bandchosen

        array set parwidgets [list x $win.xparameter y $win.yparameter]
        array set bandwidgets [list 1 $win.b1 2 $win.b2]

        #  The variables below determine which parameter/band will be
        #  filled next time around.

        set currentParameter x
        set currentBand      1

        $self highlightNextItems

        # Set event bindings on the parameter labels and widgets so that the user can
        # explicitly select the next one:

        bind $win.xparlabel   <Button-1> [mymethod setNextParameter x]
        bind $win.xparameter  <Button-1> [mymethod setNextParameter x]

        bind $win.yparlabel   <Button-1> [mymethod setNextParameter y]
        bind $win.yparameter  <Button-1> [mymethod setNextParameter y]

        # Similarly...allow the user to click the gate label/widgets to
        # select which will be replaced next.

        bind $win.b1label     <Button-1> [mymethod setNextGate 1]
        bind $win.b1          <Button-1> [mymethod setNextGate 1]

        bind $win.b2label     <Button-1> [mymethod setNextGate 2]
        bind $win.b2          <Button-1> [mymethod setNextGate 2]
    }
    # reinit
    #     Return the gate editor to its initial state:
    #     empty strings for both the parameters and gates.
    #
    method reinit {} {
        $win.xparameter configure -text $emptyString
        $win.yparameter configure -text $emptyString

        $win.b1 configure -text $emptyString
        $win.b2 configure -text $emptyString

        $win.browser update
    }
    # getDescription
    #     Returns the description string for the gate.
    #     for g2band, this is just the set of gates the user
    #     selected.
    #       If either gate is emptyString then a blank descriptor
    #     is returned.
    #
    method getDescription {} {
        set g1 [$win.b1 cget -text]
        set g2 [$win.b2 cget -text]

        if {($g1 == $emptyString) || ($g2 == $emptyString) } {
            return [list];                    #Incomplete selection
        }
        return [list $g1 $g2]
    }
    # getHelpTopic
    #     Return the appropriate help topic for this gate editor.
    #
    method getHelpTopic {} {
        return C2Gate
    }
    # -------------- Local methods and procs.

    # filterGates descr
    #     Called to veto the addition of gates to the browser.
    #     Before we allow any gates to be present in the browser,
    #     we require that the x/y parameters have been chosen.
    #     Once that's the case, we only allow the browser to display
    #     bands which are set on the parameters the user selected.
    # Parameters:
    #    gate  - the full gate description to filter.
    method filterGates gate {

        #   Both parameters must be selected to allow any gates
        #   to show.
        #
        set xp [$win.xparameter cget -text]
        set yp [$win.yparameter cget -text]
        if {($xp == $emptyString) || ($yp == $emptyString)} {
            return 0
        }
        #
        #  Only bands are allowed to show.
        #
        set type [lindex $gate 2]
        if {$type != "b"} {
            return 0
        }
        #
        #   The gate parameters must be the
        #   x/y parameter (although order can be
        #   flipped.
        #
        set descr [lindex $gate 3]
        set gpars [lindex $descr 0]
        set gx    [lindex $gpars 0]
        set gy    [lindex $gpars 1]

        if {($xp == $gx) && ($yp == $gy) } {
            return 1
        }
        if {($yp == $gx) && ($xp == $gy) } {
            return 1
        }


        return 0;    # Stub.
    }
    # highlightNextItems
    #     Highlights the items that will be replaced the next time around.
    #
    method highlightNextItems {} {
        #  First return all backgrounds to that of $win.browser

        set normalbkg [. cget -background]
        foreach sub {xparlabel yparlabel xparameter yparameter b1label b2label b1 b2} {
            $win.$sub configure -background $normalbkg
        }
        # Highlight the appropriate parameter and label.

        if {$currentParameter == "x"} {
            set sub [list xparlabel xparameter]
        } else {
            set sub [list yparlabel yparameter]
        }
        foreach wid $sub {
            $win.$wid configure -background cyan
        }
        # Highlight the appropriate band and label.


        if {$currentBand == "1"} {
            set sub [list b1label b1]
        } else {
            set sub [list b2label b2]
        }
        foreach wid $sub {
            $win.$wid configure -background cyan
        }
    }
    # setCurrentParameter name
    #     The parameter is loaded into the appropriate
    #     parameter slot and data and the current parameter is flipped.
    #     The gates folder is then updated.  If either of the current
    #     gates is not in the acceptable set of gates, it is removed.
    # Parameters:
    #    name   - Name of the new parameter.
    method setCurrentParameter name {
        set label $parwidgets($currentParameter)
        $label configure -text $name

        if {$currentParameter == "x"} {
            $self setNextParameter y
        } else {
            $self setNextParameter x
        }
    }


    # selectParameter args
    #     Called on a double click of a parameter.
    #     The appropriate parameter is set.
    #
    method selectParameter {args} {
        set selection [$win.browser getSelection]
        set name [::pathToName $selection]
        if {[parameter -list $name] != ""} {
            $self setCurrentParameter $name
            $win.browser update;             # Re filter gates
            $self checkCurrentGates;         # Ensure selected bands still valid.
            $self removeUsedItems;           # Remove the items that have been selected.

        }
    }
    # selectGate args]
    #    Called on a double click of a gate.
    #    The appropriate gate is set.
    #
    method selectGate {args} {
        set selection [$win.browser getSelection]
        set name      [::pathToName $selection]

        if {[gate -list $name] != ""} {
            $self setCurrentGate $name
            $self removeUsedItems
        }
    }
    # setNextParameter which
    #     Sets the next parameter selected to 'which'.
    # Parameters:
    #   which  - either x or y indicating which paramter is to be filled next.
    #
    method setNextParameter which {
        set currentParameter $which
        $self highlightNextItems

    }
    #  setNextGate which
    #     Sets the next gate selected to 'which'
    # Parameters:
    #   which - either 1 or 2 indicating which gate is to be filled next.
    #
    method setNextGate which {
        set currentBand $which
        $self highlightNextItems
    }
    # setCurrentGate name
    #     Sets the current gate to 'name' and flip to the next gate.
    #
    method setCurrentGate name {
        set widget $bandwidgets($currentBand)
        $widget configure -text $name

        if {$currentBand == "1"} {
            $self setNextGate 2
        } else {
            $self setNextGate 1
        }
    }
    # checkCurrentGates
    #      When parameters are modified, this
    #      is called to ensure that the current
    #      set of gates is still valid.  If a band
    #      is invalid, it is cleared back to emptyString
    #
    method checkCurrentGates {} {
        foreach i {1 2} {
            set widget $bandwidgets($i)
            set gate [$widget cget -text]
            set description [lindex [gate -list $gate] 0]
            if {![$self filterGates $description]} {
                $widget configure -text $emptyString
            }
        }
    }
    # removeUsedItems
    #      Removes items that are currently selected as parameters
    #      and gates from the browser.
    #
    method removeUsedItems {} {
        # Parameters:

        foreach i {x y} {
            set widget $parwidgets($i)
            set name [$widget cget -text]
            if {$name != $emptyString} {
                set name Parameters.$name
                $win.browser remove $name
            }
        }
        foreach i {1 2} {
            set widget $bandwidgets($i)
            set name [$widget cget -text]
            if {$name != $emptyString} {
                set name Gates.$name
                $win.browser remove $name
            }
        }
    }


}
