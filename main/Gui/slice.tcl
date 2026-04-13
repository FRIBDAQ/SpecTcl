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
#f

package provide sliceditor 1.0
package require snit
package require browser
package require guiutilities

#  The slice editor allows the user to enter a slice gate.
#  Note that since slice gates are usually most easily entered via the
#  displayer, warning text to this effect is displayed on the editor.
#
#   The megawidget is laid out as shown below:
#
#  +---------------------------------------------------------+
#  |  NOTE: Slice gates can be entered graphically as well!  |
#  |  +--------------------------+       +------------+-+    |
#  |  |   parameter browser      |       | Parameter  | |    |
#  |  |                          |       |  list      | |    |
#  |  +--------------------------+       +------------+-+    |
#  |    Low: [           ]     High:  [       ]              |
#  +---------------------------------------------------------+
# options:
#    -maxparams  n   - Maximum number of parameters the user can
#                      select for the gate.
#
snit::widget sliceEditor {
    option -maxparams 0
    constructor args {
        $self configurelist $args

        label $win.warning -foreground red -text {NOTE: Slice gates can be entered graphically as well!}

        label     $win.parlabel -text {Parameter(s) selected}
        listbox   $win.parameters -yscrollcommand  [list $win.parscroll set]
        scrollbar $win.parscroll  -orient vertical -command [list $win.parameters yview]

        browser $win.browser -restrict parameters -showcolumns [list low high bins units]     \
                             -parameterscript  [mymethod selectParameter] \
                             -filterparameters [mymethod parameterFilter]


        frame $win.limits
        label $win.limits.lowlabel -text {Low Limit: }
        entry $win.limits.low
        label $win.limits.hilabel  -text {High Limit: }
        entry $win.limits.high
        pack  $win.limits.lowlabel $win.limits.low $win.limits.hilabel $win.limits.high -side left

        # Lay this out...

        grid $win.warning -  -
        grid $win.browser $win.parlabel x -sticky s
        grid    ^         $win.parameters -sticky nsew
        grid $win.parscroll -row 2 -column 2 -sticky ns


        grid $win.limits
        
        bind $win.parameters <Double-1> [mymethod removeParameter]

    }
    ##
    # getBrowser
    #    Return the browser widget... this is intended to allow
    #    the vector slice editor to re-use bits of us.
    #
    method getBrowser {} {
        return $win.browser
    }
    ##
    # getParameterListbox
    #    Return the parameter list box.
    #
    method getParameterListBox {} {
        return $win.parameters
    }
    # load name
    #     Load the editor with the gate information from the selected gate.
    #     How this is done depends a bit on the gate type (s or gs).
    # Parameter:
    #     name   -Name of the gate to load into the editor.
    #
    method load name {
        $self reinit
        set info [gate -list $name]
        set info [lindex $info 0]
        set descr [lindex $info 3]
        set type  [lindex $info 2]
        if {($type  eq "s") || ($type eq "vs*") || ($type eq "vs+") } {
            $self addParameter [lindex $descr 0]
            set limits [lindex $descr 1]
            ::setEntry $win.limits.low  [lindex $limits 0]
            ::setEntry $win.limits.high [lindex $limits 1]
        } else {
            set limits [lindex $descr 0]
            set parameters [lindex $descr 1]
            ::setEntry $win.limits.low [lindex $limits 0]
            ::setEntry $win.limits.high [lindex $limits 1]

            foreach name $parameters {
                $self addParameter  $name
            }
        }
        $win.browser update;     # Filters

    }
    # reinit
    #    Called to initialize the editor to the initial configuration.
    #
    method reinit {} {
        $win.parameters delete 0 end
        $win.limits.low delete 0 end
        $win.limits.high delete 0 end
        $win.browser update
    }
    # getDescription:
    #     Return the gate description string.
    #     This is a Tcl list.  The form of this list
    #     depends on the value of -maxparams.
    #     -maxparams = 1:  A two element list consisting of the parameter
    #                      chosen and the low/high limit pair as  a 2 element
    #                      sublist.
    #    -maxparams > 1:  A two element list consisting of the low/high limit
    #                      pair as a 2 element sublist and the list of parameters chosen
    #                      as a sublist (gamma gate description)
    #     If no parameters are chosen or if either limit is not a double value, then
    #     an empty description is returned to flag an error.
    #
    method getDescription {} {
        #  Fetch out the user's input.

        set params [$win.parameters get 0 end]
        set low    [$win.limits.low get]
        set high   [$win.limits.high get]


        if {([llength $params]) == 0 || ![string is double -strict $low]  ||
            ![string is double -strict $high]} {
            return [list]
        }
        #  The rest is just a matter of ordering:

        if {$options(-maxparams) == 1} {
            return [list $params [list $low $high]]
        } else {
            return [list [list $low $high] $params]
        }

    }
    # getHelpTopic
    #      Returns the help topic for this editor.
    #
    method getHelpTopic {} {
        return sliceGate
    }
    # selectParameters args
    #      Adds the parameter selected to the parameter list box.
    #      If the number of parameters in the listbox would exceed -maxparams,
    #      the first entry is removed.
    #      The browser is then refreshed, allowing the filter to remove
    #      the parameters that have been selected and to display those
    #      that may have been removed from the box.
    method selectParameter {selection} {

        if {$selection == ""} return
        set atmost $options(-maxparams)
        foreach item $selection {
            set parameter [::pathToName $item]
            $win.browser deleteElement parameter $parameter ;   #Remove from tree.
            set oldValue [$self addParameter $parameter]
        
            if {($atmost != 0) && ([$win.parameters index end] > $atmost)} {
                $win.browser addNewParameter $oldValue ;  # add back to tree.
            }
        }
        # $win.browser update
    }
    ##
    # addParameter
    #    Adds an item to the parameter list box, enforicing the maximum.
    # @param parameter - parameter to add.
    #
    method addParameter parameter {
        set oldValue ""
        set atmost $options(-maxparams)
        $win.parameters insert end $parameter
            if {($atmost != 0) && ([$win.parameters index end] > $atmost)} {
                set oldValue [$win.parameters get 0]
                $win.parameters delete 0
            }
        return $oldValue
    }
    # paramterFilter desc
    #      Determines if the parameter described should appear in the browser.
    #      The parameter will only appear if it is not also in the $win.parameters
    #      listbox.
    # Parameters:
    #    desc  - Description of the paramter to checkk.
    # Returns:
    #     0   - Don't show.
    #     1   - Show.
    #
    method parameterFilter descr {
        set parameters [$win.parameters get 0 end]
        set name       [lindex $descr 0]
        if {[lsearch -exact $parameters $name] != -1} {
            return 0
        } else {
            return 1
        }
    }
    # #
    # removeParameter
    #    Called when a parameter in the gate is double clicked.
    #    - The selected parameter is removed from the list box.
    #    - The selected parameter is added back to the browser.
    #
    method removeParameter {} {
        
        #  Get the list of parameters selected in the list box
        #  ids and names.   If none are selected the double click
        #  happened in empty space and nothing is done:
        
        set ids [$win.parameters curselection]
        if {[llength $ids] == 0} {
            return
        }
        #  Just in case sort the ids numerically descending so they
        #  can be deleted from the list box without invalidating the
        #  other indices:
        
        set ids [lsort -integer -decreasing $ids]
        
        #  Delete the list box elements 
    
        foreach id $ids {
            set name [$win.parameters get $id]
            $win.browser addNewParameter $name;   # Add back to tree.
            $win.parameters delete $id
        }
       
        # updateing the browser for now will show the removed params
        # because they will no longer be filtered out:
        
        # $win.browser  update
    }
}

###
#  Vector gate editor.
#   We leverage as much of the slice editor as possible. The 
#   Difference is that we are going to only show vectors in
#   the browser and we'll filter out all of the vectors in 
#   the list box.
#
snit::widgetadaptor vectorSliceEditor {
    variable browser
    variable params


    delegate option * to hull
    delegate method * to hull
    
    constructor {args} {
        installhull using sliceEditor
        $self configurelist $args

        set browser [$hull getBrowser]
        set params  [$hull getParameterListBox]

        $hull configure -maxparams 1;                # only one vector allowed.

        # Now set up the browser to only show vectors and to
        # use my filter for them.  Establish my handler for vector selection:

        $browser configure -restrict vectors -vectorscript [mymethod _selectVector] \
            -filtervectors [mymethod _vectorFilter] -selectmode browse

        # Replace the double click binding on the params listbox to do our bidding

        bind $params <Double-1> [mymethod _removeVector]
            
    }

    ##
    # _selectVector
    #    Called when a vector is selected from the 
    #    browser tree:
    # - If there's already one in the list box it's removed and added back to the tree.
    # - The vector is removed from the browser and put in the list box.
    #
    # @param selection - the path to the  to the item int he tree:
    #
    method _selectVector selection {
        
        if {$selection == ""} return;                  # nothing selected.
        if {[llength $selection] > 1} {
            # Shouldn ot be possible but flag it as an error:

            tk_messageBox -title "Invalid seletion" -message "Please only select one item" -icon warning -type ok
            return
        }
        set vector [::pathToName $selection]
        $browser deleteElement vectors $vector;    # remove from the browser.
        set oldItem [$params get 0]
        if {$oldItem ne ""} {
            #  Add the item int he box back to the tree:

            $browser addNewVector $oldItem
        }
        #  Kill off anything in the list box and add us:

        $params delete 0 end
        $params insert end $vector
       
    }
    ##
    # _vectorFilter
    #    IF there's anything in the listbox, don't let it show in the browser.
    #
    # @param def - a vector definition.
    # @return bool - true if def shouild be in the list box.
    #
    method _vectorFilter def {
        
        set listitem [$params get 0]
        if {$listitem ne ""} {
            set vectorName [dict get $def name]
            return [expr {$vectorName ne $listitem}]
        } else {
            return 1
        }
    }
    ##
    # _removeVector
    #   Remove any vector from the list box.  This is simpler than the sliceEditor because we can
    # only have one item in the box.    

    method _removeVector {} {

        set vector [$params get 0]
        $params delete 0 end
        if {$vector ne ""} {
            $browser addNewVector $vector
        }
    }
}
