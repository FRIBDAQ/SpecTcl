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
package provide edit1d 1.0
package require snit
package require guiutilities

# Provides an editor for 1-d spectra.
#
snit::widget edit1d {
    option -browser {}

    variable emptyString {<............>}

    constructor args {
        $self configurelist $args

        set browser $options(-browser)

        $browser configure -parameterscript  [mymethod selectParameter] \
           -filterparameters [mymethod filterParams]
        # myframe has all my components.
        # browser will eventually be gridded -into $win.
        #
        set myframe [frame $win.myframe -relief groove -borderwidth 3]

        # The top two rows are for the parameter

        label $myframe.parlabel  -text {Parameter}
        label $myframe.parameter -text $emptyString

        # The next row labels the entries and units label.

        label $myframe.lolabel   -text {Low}
        label $myframe.hilabel   -text {High}
        label $myframe.binlabel  -text {Bins}
        label $myframe.unitlabel -text {Units}

        # The bottom row has the entries and units label:

        entry $myframe.low    -width 5
        entry $myframe.high   -width 5
        entry $myframe.bins   -width 5
        label $myframe.units -text {       }

        # Layout stuff in myframe then paste the browser and myframe
        # side by side.

        grid $myframe.parlabel
        grid $myframe.parameter

        grid $myframe.lolabel $myframe.hilabel $myframe.binlabel $myframe.unitlabel
        grid $myframe.low     $myframe.high    $myframe.bins     $myframe.units

        pack $browser $myframe  -side left -fill y -expand 1
    }
    # reinit
    #    Called by clients to reinitialize the editor to an 'unfilled in'
    #    state
    method reinit {} {
        set myframe $win.myframe
        $myframe.parameter configure -text $emptyString

        ::setEntry $myframe.low {}
        ::setEntry $myframe.high {}
        ::setEntry $myframe.bins {}

        $myframe.units configure -text {}
    }
    # load name
    #      Load the indicated spectrum into the editor.
    #      it is up to the caller to ensure this is a 1-d spectrum.
    # Parameters:
    #   name   - The spectrum name.
    #
    method load name {
        $self reinit

        set info [spectrum -list $name]
        set info [lindex $info 0]
        set parameter [lindex $info 3]
        set axis      [lindex $info 4]
        set axis      [lindex $axis 0]
        set low       [lindex $axis 0]
        set high      [lindex $axis 1]
        set bins      [lindex $axis 2]
        set units     {}

        set paraminfo [treeparameter -list $parameter]
        if {[llength $paraminfo] != 0} {
            set paraminfo [lindex $paraminfo 0]
            set units     [lindex $paraminfo 5]
        }
        $self loadParameterInfo $parameter $low $high $bins $units


        $options(-browser) update
    }
    #getHelpTopic
    #    Return the help topic for this version of the dialog.
    #
    method getHelpTopic {} {
        return spectrum1d
    }
    # getParameters
    #      Returns the name of the parameter chosen by the user
    #      for this spectrum.  This will be empty if one has not
    #      yet been chosen.
    #
    method getParameters {} {
        set parName [$win.myframe.parameter cget -text]
        if {$parName == $emptyString} {
            return [list]
        } else {
            return [list $parName]
        }
    }
    # getAxes
    #     Returns the 'list' of axis descriptions for this
    #     spectrum.   If the user has not filled in everything,
    #     this can also be blank...also if the user has
    #     filled in some bad values...
    #
    method getAxes {} {
        set myframe $win.myframe
        set low  [$myframe.low get]
        set high [$myframe.high get]
        set bins [$myframe.bins get]

        #  Must be strictly doubles:

        if {[string is double -strict $low] && [string is double -strict $high] && [string is integer -strict $bins]} {
            return [list [list $low $high $bins]]
        } else {
            return [list]
        }
    }
    # selectParameter name
    #      Called to select a parameter from the browser.
    #      The parameter is loaded into the parameter name label.
    #      if the parameter is a treeparam it's information is
    #      loaded into the entry widgets.
    # Parameters:
    #   name - Full path to the clicked item (e.g. Paramters.event...).
    #
    method selectParameter {name} {

        set name [::pathToName $name]

        set info [treeparameter -list $name]
        if {[llength $info] != 0} {
            set info [lindex $info 0]
            set low  [lindex $info 2]
            set high [lindex $info 3]
            set bins [lindex $info 1]
            set units [lindex $info 5]

        } else {
            set low {}
            set high {}
            set bins {}
            set units {}
        }
        $self loadParameterInfo $name $low $high $bins $units

    }
    #  loadParameterInfo   name low high bins units
    #      Loads the gui with parameter information.
    #
    method loadParameterInfo {name low high bins units} {
        set browser $options(-browser)
        set myframe $win.myframe
        set oldParam [$myframe.parameter cget -text]
        set oldDesc [parameter -list $oldParam]
        if {[llength $oldDesc] > 0} {
            $browser addNewParameter $oldParam    
        }
        
        $browser deleteElement parameter $name
        $myframe.parameter configure -text $name
        ::setEntry $myframe.low  $low
        ::setEntry $myframe.high $high
        ::setEntry $myframe.bins $bins
        $myframe.units configure -text $units

    }
    ##
    # filterParams
    #   Only allow the tree to have parameters that are not
    #   the one in the spectrum.
    #
    # @param desc  - parameter descriptor.
    #
    method filterParams desc {
        set p [lindex $desc 0]
        set text [$win.myframe.parameter cget -text]
        if {$p == $text} { return 0 } else { return 1 }
    }
}
