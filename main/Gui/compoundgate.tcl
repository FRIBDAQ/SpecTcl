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

package provide compoundgate 1.0
package require snit
package require browser
package require BWidget
package require guiutilities

# This package provides a compound gate editor megawidget.
# Options:
#    -atmost n       - Limits the number of gate dependencies allowed
#                      if 0 there is no limit.
# The layout is something like:
#    +---------------------------------------------------------+
#    | +---------------------+            +----------------+   |
#    | |    gate browser     |            |                |   |
#    | |       (browser)     |            |  scrolling     |   |
#    | |                     |   >        |  listbox       |   |
#    | |                     |   <        |                |   |
#    | +---------------------+            +----------------+   |
#    +---------------------------------------------------------+
#
#    The > < are arrow buttons:
#        >    Moves the selected gate from the gate browser to
#             list of gates in the listbox.
#        <    Moves the selected gate in the gate browser
#             back to the gate browser.
#    If -atmost is nonzero, adding extra gates to the listbox
#    will move a gate from the listbox back to the browser if
#    the number of gates in the listbox would otherwise exceed
#    the atmost value.
#
#
snit::widget compoundgate {
    option -atmost 0
    option -dependentlabel {Dependent Gates}
    variable gateName {<........>}

    constructor {args} {
        $self configurelist $args

        # Labels for the browser and the listbox:

        label $win.blabel -text "Available Gates"
        label $win.dlabel -text $options(-dependentlabel)


        listbox $win.dependencies   -yscrollcommand "$win.scrolldep set"
        scrollbar $win.scrolldep    -command        "$win.dependencies yview"

        #  The browser will be restricted to gates and not show details:
        #
        browser $win.browser -restrict gates -detail 0 -showcolumns type -width 4in \
                             -filtergates [mymethod gateFilter]


        grid $win.blabel    $win.dlabel
        grid $win.browser          -row 1 -column 0 -rowspan 2
        grid $win.dependencies     -row 1 -column 1 -rowspan 2 -sticky nsew
        grid $win.scrolldep        -row 1 -column 2 -rowspan 2 -sticky ns


        # Event bindings below are short cuts for the user.. double clicking an item
        # in a box moves it to the other box... if its movable.

        bind $win.dependencies <Double-1>  [mymethod removeFromListbox]
        $win.browser configure -gatescript [mymethod addToListbox]

    }
    # load name
    #      Load the editor with the current set of parameters in the
    #      named gate.  It is the user's responsibility to determine that
    #      the gate is a compound gate.
    #
    # Parameter:
    #     name  - Name of the gate to load.
    #
    method load name {
        set gateName $name
        set info [gate -list $name]
        set info [lindex $info 0]
        set subgates [lindex $info 3]
        $win.dependencies delete 0 end
        foreach gate $subgates {
            $win.dependencies insert end $gate
        }
        $win.browser update
    }
    # reInit
    #     Reinitializes the widget to make it possible to start a new gate.
    #
    method reinit {} {
        $win.browser update
        $win.browser update
        $win.dependencies delete 0 end
    }
    # getDescription
    #     Returns the description list of the gate that has been defined so far.
    #
    method getDescription {} {
        return [$win.dependencies get 0 end]
    }
    # getHelpTopic
    #     Returns the help topic
    #
    method getHelpTopic {} {
        return compoundGate
    }
    #----------------------- Private procs and methods.

    # addToListbox
    #    Called in response to a click on the right arrow button to move the elements
    #    in the current selection of the browser over into the listbox.
    #    note that it's possible to have selected a non gate (e.g. a folder) so we double
    #    check that the gate is a legitimate gate before moving it.
    #
    method addToListbox {selection} {
        if {$selection == ""} return
        set atmost $options(-atmost)
        foreach item $selection {
            set gate [::pathToName $item]
            if {([gate -list $gate] != "") && ($gate ni [$win.dependencies get 0 end])} {
                $win.dependencies insert end $gate  
                if {($atmost > 0) && ([$win.dependencies index end] > $atmost) } {
                    # Prune back the oldest..

                    set loser [$win.dependencies get 0]
                   # $win.browser addGate $loser
                    $win.dependencies delete 0
                }
            }
        }
        $win.browser update
    }
    # removeFromListbox
    #     Called in respones to a click on the left arrow button.
    #     The selected elements of the gate dependency listbox are
    #     removed and added to the gate browser.
    #
    method removeFromListbox {} {

        #
        #  Get the selection list in decreasing order so
        #  indices don't change as we remove them.
        #
        set selection [$win.dependencies curselection]
        set selection [lsort -decreasing -integer $selection]
        foreach id $selection {
            set gatename [$win.dependencies get $id]
            $win.dependencies delete $id
           # $win.browser addGate $gatename
        }
        $win.browser update;         # Update the gates.
    }
    # gateFilter descr
    #       Determins if a gate should be displayed in the
    #       browser.  Gate are displayed if they are not
    #       in the dependcy set.
    # Parameters:
    #    descr - SpecTcl gate description.
    method gateFilter descr {
        set name [lindex $descr 0]
        set dependencies [$self getDescription]
        lappend dependencies $gateName

        if {[lsearch -exact $dependencies $name] != -1} {
            return 0
        } else {
            return 1
        }
    }

    #  gateToName name
    #      Turns a gate into a nodename for the browser:
    # Parameters:
    #   name  - The name of a gate.
    # Returns:
    #   A good name for the gate in the browser.
    proc gateToName name {
        append result Gate . $name
        return $result
    }

}
