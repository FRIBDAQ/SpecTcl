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

package provide editmulti 1.0
package require snit
package require browser
package require guiutilities

# Provides a spectrum editor for spectra that accept
# an unbounded list of parameters.  These include gamma
# spectra and summary spectra.  At this time we support
# an option:
#    -axes    1 or 2 specifies the number of axes
#             definition prompters that will be displayed.
#  The layout of the editor looks like this:
#
#  +-------------------------------------------------------+
#  | +-------------------------+   +-----------------+-+   |
#  | | Parameter/gate browser  |   | parameter       | |   |
#  | |                         |   | list box        | |   |
#  | |                         |   +-----------------+-+   |
#  | |                         |   { 1 or 2 axis        }  |
#  | |                         |   { prompters          }  |
#  | +-------------------------+                           |
#  +-------------------------------------------------------+
#
#  Where an axis prompter consists of labels and entries
#  for low, high and bins.
#  All axis prompters are loaded by default with the first
#  parameter selected if it is a treeparameter otherwise
#  they are left alone.
#
#
snit::widget editmulti {
    option -axes 1
    option -browser {}

    constructor args {
        $self configurelist $args

        # Validate the -axis count.

        set axisCount $options(-axes)
        if { ($axisCount < 1)  || ($axisCount > 2)} {
            error "-axis value: $axisCount must be 1 or 2."
        }

        # Create the gui:

        set myframe [frame $win.myframe -borderwidth 3 -relief groove]
        set browser $options(-browser)

        label     $myframe.parlabel       -text {Parameters}
        listbox   $myframe.parameters     -yscrollcommand [list $myframe.scroll set]
        scrollbar $myframe.scroll -orient vertical -command [list $myframe.parameters yview]
        label     $myframe.empty   -text {}; # to get spacing before axes (kludgy I know).

        for {set axis 1} {$axis <= $axisCount} {incr axis} {
            set axisframe [frame $myframe.axis$axis]

            if {$axis == 1} {
                label $axisframe.axisname -text {X Axis definition}
            } else {
                label $axisframe.axisname -text {Y Axis definition}
            }

            label $axisframe.lowlbl  -text Low
            label $axisframe.hilbl   -text High
            label $axisframe.binlbl  -text Bins
            label $axisframe.unitlbl -text Units

            entry $axisframe.low     -width 5
            entry $axisframe.high    -width 5
            entry $axisframe.bins    -width 5
            label $axisframe.units   -text {     }

            grid $axisframe.axisname -                   -                -
            grid $axisframe.lowlbl $axisframe.hilbl $axisframe.binlbl $axisframe.unitlbl
            grid $axisframe.low    $axisframe.high  $axisframe.bins   $axisframe.units

        }
        # now layout the gui:

        grid $myframe.parlabel    -
        grid $myframe.parameters $myframe.scroll -sticky ns
        grid $myframe.empty       -
        for {set axis 1} {$axis <= $axisCount} {incr axis} {
            grid $myframe.axis$axis -
        }
        pack $browser $myframe -side left -expand 1 -fill y

        # Event bindings.

        bind $myframe.parameters <Double-1> [mymethod  removeParameter $myframe.parameters %x %y]


        # Browser configuration.

        $browser configure -filterparameters [mymethod parameterFilter]        \
                           -parameterscript  [mymethod addParameter]
    }
    #              ---- Methods to query megawidget state ----

    # reinit
    #    Set the Gui to an un-edited state.
    method reinit {} {
        $win.myframe.parameters delete 0 end
        $options(-browser)    update
        for {set axis 1} {$axis <= $options(-axes)} {incr axis} {
            set faxis $win.myframe.axis$axis
            ::setEntry $faxis.low {}
            ::setEntry $faxis.high {}
            ::setEntry $faxis.bins {}
            $faxis.units configure -text {     }
        }

    }
    # load name
    #      Loads the editor with the spectrum described by name.
    # Parameters:
    #  name   - Name of the spectrum to load.
    #
    method load name {
        $self reinit

        set info [spectrum -list $name]
        set info [lindex $info 0]

        set parameters [lindex $info 3]
        set axes       [lindex $info 4]

        foreach parameter $parameters {
            $win.myframe.parameters insert end $parameter
        }
        set units {}
        set keyparam [lindex $parameters 0]
        set paraminfo [treeparameter -list $keyparam]
        if {[llength $paraminfo] != 0} {
            set paraminfo [lindex $paraminfo 0]
            set units     [lindex $paraminfo 5]
        }

        set id 1
        foreach axis $axes {
            set low  [lindex $axis 0]
            set high [lindex $axis 1]
            set bins [lindex $axis 2]
            $self setAxis $id $low $high $bins $units
            incr id
        }

        $options(-browser) update
    }
    # getHelpTopic
    #     Does exactly that.
    #
    method getHelpTopic {} {
        return multiparam
    }
    # getParameters
    #      Return the list of parameters that have been selected
    #      for this spetrum by the user.
    #
    method getParameters {} {
        set parameters [$win.myframe.parameters get 0 end]
        #
        #  Require at least as many parameters as axes.
        #
        if {[llength $parameters] < $options(-axes)} {
            return [list]
        }
        return $parameters
    }
    # getAxes
    #     Return the set of axes that have been provided by the user.
    #     We will return blank if any of the values is not valid for
    #     either axis.
    #
    method getAxes {} {
        for {set axis 1} {$axis <= $options(-axes)} {incr axis} {
            set frame $win.myframe.axis$axis
            set low  [$frame.low get]
            set high [$frame.high get]
            set bins [$frame.bins get]

            if {![string is double -strict $low] || ![string is double -strict $high] ||
                ![string is double -strict $bins]} {
                return [list]
            }
            lappend description [list $low $high $bins]
        }
        return $description
    }
    #             ---- Internal methods ----

    # removeParameter widget x y
    #       Removes the paramter under the cursor from the
    #       parameter list.  We also force a browser update
    #       as this changes the filter a bit.
    # Parameters:
    #  widget   - list box containing the current parameters.
    #  x,y      - The pointer coordinates.
    #
    method removeParameter {widget x y} {
        $win.myframe.parameters delete @$x,$y
        $options(-browser) update
    }
    # parameterFilter description
    #      Determines if the parameter described deserves to appear in
    #      in the browser.   If the parameter is in the parameters listbox
    #      it will >not< appear.
    # Parameters:
    #   description - SpecTcl description of the parameter.
    method parameterFilter description {
        set name [lindex $description 0]
        set accepted [$win.myframe.parameters get 0 end]

        if {[lsearch -exact $accepted $name] == -1} {
            return 1
        } else {
            return 0
        }

    }
    # addParameter path
    #      Add the parameter to the set of parameters
    #      the user has selected for the spectrum.  If this is
    #      the first parameter, and it is a treeparam, then the
    #      axes are loaded from its information.
    #
    # Parameters:
    #   path  - The full path to the entry selected (e.g. has the Parameter. prefix).
    #
    method addParameter {path} {
        set name [::pathToName $path]
        $win.myframe.parameters insert end $name

        if {[$win.myframe.parameters index end] == 1} {
            $self setAxisDefinitions $name
        }
        $options(-browser) update
    }
    # setAxisDefinitions name
    #      If the given parameter name is a treeparam;
    #      set the axis definition(s) to its information.
    # Parameters:
    #    name  - The name  of the parameter.
    method setAxisDefinitions name {
        set axisCount $options(-axes)
        set info [treeparameter -list $name]
        if {[llength $info] != 0} {
            set info  [lindex $info 0]
            set bins  [lindex $info 1]
            set low   [lindex $info 2]
            set high  [lindex $info 3]
            set units [lindex $info 5]
            for {set axis 1} {$axis <= $axisCount} {incr axis} {
                $self setAxis $axis $low $high $bins $units
           }
        }
    }
    # setAxis  which low high bins units
    #       Sets an axis definition into the
    #       editor.
    # Parameters:
    #   which  -- Which axis to set (1/2).
    #   low    -  Low limit
    #   hight  - High limit.
    #   bins   - Number of bins.
    #   units  - Axis units.
    #
    method setAxis {which low high bins units} {
        set frame $win.myframe.axis$which
        ::setEntry $frame.low $low
        ::setEntry $frame.high $high
        ::setEntry $frame.bins $bins
        $frame.units configure -text $units

    }

}
