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
package provide filtercontrol 1.0
package require snit
package require browser
package require guiutilities
package require Iwidgets
package require Tktable

#  Returns the state of a named filter.
#
proc filterState name {
    set info [filter -list $name]
    set info [lindex $info 0]
    set state [lindex $info 4]
    if {$state == "enabled"} {
        return 1
    } else {
        return 0
    }
}

# createFilter
#       Supports the GUI based creation of a new filter.
#       This is done as a wizard with the following steps:
#       - Filter name selection.
#       - Gate selection.
#       - Parameter selection.
#       - Output file and initial state (enabled or not).
#       The wizard is implemented as a stateful modal megawizard
#       When done, the filtername, gate name, output parameters, output file
#       and initial enable state can be fetched from the wizard.
#       If the filter is completely specified, it is created (or deleted and then
#       created if necessary).
#
proc createFilter {} {
    filterwizard .filterwizard
    .filterwizard modal
    if {[winfo exists .filterwizard]} {;                   # could have x'd it.
        if {[.filterwizard cget -filtername] != ""} {;     # cancelled if "".
            set name       [.filterwizard cget -filtername]
            set gate       [.filterwizard cget -gatename]
            set parameters [.filterwizard cget -parameters]
            set file       [.filterwizard cget -filepath]
            set state      [.filterwizard cget -active]

            # If the filter exists, give the guy a chance to veto the
            # Replacement:
            #
            if {[filter -list $name] != ""} {
                set ok [tk_messageBox -icon info -title {Existing filter} -type okcancel \
                           -message "The filter $name already is defined do you want to replace it?"]
                if {$ok == "cancel"} {
                    destroy .filterwizard
                    return
                }
                catch {filter -disable $name};            # flushes/closes the output file.
                filter -delete $name
            }
            filter $name $gate $parameters
            filter -file $file $name
            if {$state} {
                filter -enable $name
            }
        }
        failsafeWrite
        destroy .filterwizard
    }
}
# enableFilters
#         Invoke the filter enable dialog.
#         Once that has exited modify the enable
#         state of the filters.
#
proc enableFilters {} {
    if {[winfo exists .filterenables]} {
        return
    }
    filterenables .filterenables
    .filterenables modal
    if {![winfo exists .filterenables]} return

    set enabledFilters  [.filterenables cget -enabledfilters]
    set disabledFilters [.filterenables cget -disabledfilters]


    foreach filter $enabledFilters {
        set enabled [filterState $filter]
        if {!$enabled} {
            filter -enable $filter
        }
    }
    foreach filter $disabledFilters {
        set enabled [filterState $filter]
        if {$enabled} {
            filter -disable $filter
        }
    }
    failsafeWrite;                   # In case I ever save filter enable state.
    destroy .filterenables
}
# filterenables
#      Provides a dialog that allows the user to set and clear
#      the state of each filter enable.
# options:
#     -enabledfilters           - Get only: list of filters currently enabled.
#     -disabledfilters          - Get only: list of filters currently disabled.
#     -okcommand                - Called when ok is clicked.
#     -cancelcommand            - Called when cancel is clicked.
#
# Widget Layout:
#     +-----------------------------------------+
#     |  +-----------------------------------+  |
#     |  | enable | Name | Gate | parameters |  |
#     |  +-----------------------------------+  |
#     |  |  Table of gates with checkbuttons |  |
#     |  | in the enable column.             |  |
#     |  +-----------------------------------+  |
#     |    <Ok>    <Cancel>                     |
#     +-----------------------------------------+
snit::widget filterenables {
    hulltype toplevel

    option -enabledfilters
    option -disabledfilters
    option -okcommand {}
    option -cancelcommand {}

    variable cancelled 0
    variable hidden {}

    constructor args {
        table      $win.filters -cols 7 -rows 1 -titlerows 1 -yscrollcommand [list $win.scroll set] \
                                -justify left -cache 1 -resizeborders both
        scrollbar  $win.scroll  -orient vertical -command [list $win.filters yview]

        button $win.ok     -text Ok     -command [mymethod onOk]
        button $win.cancel -text Cancel -command [mymethod onCancel]
        button $win.help   -text Help   -command [list spectclGuiDisplayHelpTopic filterEnables]

        grid $win.filters    -row 0 -column 0 -sticky nsew
        grid $win.scroll     -row 0 -column 1 -sticky nsw
        grid $win.ok $win.cancel $win.help

        $self fillTable $win.filters

        $self configurelist $args
    }
    #  fillTable widget
    #            Fills the filter table.
    # Parameters:
    #    widget    - the table widget.
    #
    method fillTable widget {

        # The titles:

        set col 0
        foreach title {Enable Name Gate File Parameters  State Format} {
            $widget set 0,$col $title
            incr col
        }
        #  Now the filters

        set row 1
        $widget tag configure invariant -state disabled

        set maxname 0
        set maxgate 0
        set maxfile 0
        set maxparam 0

        foreach filter [filter -list] {
            set name [lindex $filter 0]
            set maxname [expr max($maxname, [string length $name])]

            set gate [lindex $filter 1]
            set maxgate [expr max($maxgate, [string length $gate])]

            set file [lindex $filter 2]
            set maxfile [expr max($maxfile, [string length $file])]

            set parameters [lindex $filter 3]
            foreach parameter $parameters {
                set maxparam [expr max($maxparam, [string length $parameter])]
            }

            set height [llength $parameters]
            set parameters [join $parameters "\n"]
            set state [lindex $filter 4]
	    
	    set format [lindex $filter 5]

            $widget insert rows end
            $widget height $row $height
            $widget set row $row,1 [list $name $gate $file $parameters $state $format]

            checkbutton $widget.enable$name
            if {$state == "enabled"} {
                $widget.enable$name select
            } else {
                $widget.enable$name deselect
            }
            $widget window configure $row,0 -window $widget.enable$name

            for {set col 1} {$col < 6} {incr col} {
                $widget tag cell invariant $row,$col
            }
            incr row
        }
        $widget width 1 $maxname
        $widget width 2 $maxgate
        $widget width 3 $maxfile
        $widget width 4 $maxparam
    }
    #  onOk
    #       Called when the ok command is clicked. invoke the use script and,
    #       if necessary, destroy the hidden window  to break out of modality.
    #
    method onOk {} {
        set script $options(-okcommand)
        if {$script != ""} {
            eval $script
        }

        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }
    }
    #  onCancel
    #        Set the cancel flag so the cgetters will know what to do.
    #        If a -cancelcommand  was configured call it.
    #        If the dialog was modal, destroy the hidden window to end the grab.
    #
    method onCancel {} {
        set cancelled 1

        set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }

        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }
    }
    # modal
    #     Make the dialog modal.
    method modal {} {
        set hidden [frame $win.hidden]
        wm deiconify $win
        focus $win
        grab  $win
        tkwait window $hidden

    }
    # configure -enabledfilters -- this is illegal.
    onconfigure -enabledfilters list {
        error {-enabledfilters is a readonly option}
    }
    # configure -disabledfilters -- this is illegal
    onconfigure -disabledfilters list {
        error {-disabledfilters is a readonly option}
    }
    # cget -enabledfilters
    # Get the set of filters that the user wants to enable.
    # Note that if cancelled is true, this is an empty list
    # regardless of the checkboxes.
    oncget -enabledfilters {
        if {$cancelled} {
            return [list]
        }
        set result [list]
        foreach filter [filter -list] {
            set name [lindex $filter 0]
            set varname ::enable$name
            if {[set $varname]} {
                lappend result $name
            }
        }
        return $result
    }
    # cget -disabledfilters
    #    Same as for -enabledfilters but gets the filters that
    #    are not enabled.
    #
    oncget -disabledfilters {
        if {$cancelled} {
            return [list]
        }
        set result [list]
        foreach filter [filter -list] {
            set name [lindex $filter 0]
            set varname ::enable$name
            if {![set $varname]} {
                lappend result $name
            }
        }
        return $result
    }

}

# filterwizard
#      This is a wizard that contains 4 substates that prompt for a filter.
#      The states allow the user to, in turn set the filter name, select the gate
#      that makes up the filter.  Select the parameters that will be written
#      by the filter, and set the outputfile and initial activation state.
#
#      The states are represented by sub-dialogs that are made sequentially visible in the
#      action part of the dialog.
# The typical layout is:
#       +----------------------------------------------+
#       | Action part... contents depend on state      |
#       |                                              |
#       +----------------------------------------------+
#       |  [cancel]    [< Back]   [Next >]             |
#       +----------------------------------------------+
#  Note however:
#      - In the first state, the Back button is inactive.
#      - In the last state, the Next > button is relabeled Finish
#      - Clicking the cancel button ensures that all of the option
#        fetches return empty strings.#
# Options:
#     -filtername   Set initial filter name, or get current name.
#     -gatename     Set the initial filter gate name, or get the current gate name.
#     -parameters   Set an initial list of parameters or get the current parameter list.
#     -filepath     Set initial filter path or get the current filter file path.
#     -active       Set initial active state (0 off 1 on), or get the current state.
#     -cancelcommand Script to execute when the dialog is cancelled.
#     -finishcommand Script to execute when the dialog is 'finished'
#
snit::widget filterwizard {
    hulltype toplevel

    option -cancelcommand
    option -finishcommand


    variable dialogList {selectFilterName  selectFilterGate selectFilterParameters setFileAndState}
    variable helpTopics {nameFilter        gateFilter       setParameters          setFilterFile}
    variable wizardState 0;                          # Where in the wizard we are.
    variable hidden     {}

    # Option delegation:

    delegate option -filtername to selectFilterName
    delegate option -gatename   to selectFilterGate
    delegate option -parameters to selectFilterParameters
    delegate option -filepath   to setFileAndState
    delegate option -active     to setFileAndState

    constructor args {
        frame $win.action
        frame $win.command -borderwidth 3 -relief groove

        button $win.command.cancel -text Cancel    -command [mymethod onCancel]
        button $win.command.help   -text Help      -command [mymethod onHelp]
        button $win.command.back   -text {< Back}  -command [mymethod onBack]
        button $win.command.next   -text {Next >}  -command [mymethod onNext]
        grid $win.command.cancel $win.command.help $win.command.back $win.command.next

        pack $win.action  -expand 1 -fill x
        pack $win.command -expand 1 -fill x

        #   Create the subwidgets

        foreach state $dialogList {
            install $state using $state $win.action.$state
        }


        #   Set the current state and adjust the buttons.
        #

        $self selectState
        $self adjustCommandButtons
        $self configurelist $args

    }

    # onCancel   - Cancel the dialog by setting cancelled true.
    #              Invoke the -cancelcommand script
    #              if the dialog is modal, dismiss it.
    #
    method onCancel {} {

        if {$options(-cancelcommand) != ""} {
            eval $options(-cancelcommand)
        }

        $self configure -filtername {}

        if {[winfo exists $hidden]} {
            destroy $hidden
            set hidden {}
        }
    }
    # onHelp
    #       Displays the help text associated with the current stage of the wizard.
    #
    method onHelp {} {
        set topic [lindex $helpTopics $wizardState]
        spectclGuiDisplayHelpTopic $topic
    }
    # onBack   - Called when the back button was clicked to
    #            go to the prior state... unless we're already
    #            at state 0 in which case this is a no-op.
    #
    method onBack {} {
        if {$wizardState > 0} {
            incr wizardState -1
            $self selectState
            $self adjustCommandButtons
        }
    }
    # onNext   - Called when the next button is clicked to
    #            advance to the next state.  If the current
    #            state is the last state then -finishcommand
    #            is invoked if not null and the modal member
    #            is forced to exit.
    #
    method onNext {} {
        # Ensure the state is ok with us continuing:

        set priorstate [lindex $dialogList $wizardState]
        set message [$win.action.$priorstate nextOk]
        if {$message != ""} {
            tk_messageBox -title {Fill it all in} -icon error \
                -message $message
            return
        }

        if {$wizardState < ([llength $dialogList] -1)} {
            incr wizardState
            $self selectState
            $self adjustCommandButtons
        } else {
            if {$options(-finishcommand) != ""} {
                eval $options(-finishcommand)
            }
            if {[winfo exists $hidden]} {
                destroy $hidden
                set hidden {}
            }
        }
    }

    # adjustCommandButtons
    #     Adjusts the contents of the command buttons to reflect
    #     the current state.
    #
    method adjustCommandButtons {} {
        #
        #  First state disables back.
        #  but Next is Next.
        #
        if {$wizardState == 0} {
            $win.command.back configure -state disabled
        } else  {
            $win.command.back configure -state normal
        }
        #  Last state re-labels the next button.
        if {$wizardState == ([llength $dialogList] - 1)} {
            $win.command.next configure -text {Finish}
        } else {
            $win.command.next configure -text {Next >}
        }
    }
    # selectState
    #         unpaste the prior state dependent part of the dialog
    #         and paste the state that corresponds to the current state.
    #
    method selectState {} {
        set oldwidget [pack slaves $win.action]
        foreach widget $oldwidget {
            pack forget $oldwidget
        }
        set state [lindex $dialogList $wizardState]
        pack $win.action.$state -expand 1 -fill x
    }
    # modal
    #     Turns the filter wizard into a modal dialog.
    #     The dialog terminates when a hidden window is
    #     destroyed.
    method modal {} {
        set hidden [frame $win.hidden]
        wm deiconify $win
        focus $win
        grab $win

        tkwait window $hidden

    }
}
#  selectFilterName is a subwidget of the filter wizard that prompts
#  the user for a filter name.  In addition explanatory text is given
#  about the wizard and what the user is about to be subjected to.
#  A plain old entry wizard is used to allow the user to enter the
#  filter name.
#  Dialog layout:
#    +------------------------------------------------+
#    |    Explanatory message                         |
#    | Filter Name:    [                   ]          |
#    +------------------------------------------------+
#
#  options:
#    -filtername     Set/get the entry contents.
#
#
snit::widget selectFilterName {
    option -filtername {}

    constructor args {
        #
        message $win.info -text {Welcome to the filter creation wizard.
The filter creation wizard helps you to create SpecTcl
filters. SpecTcl filters evaluate a gate for each event.
When the gate is true a subset of the parameters are
written to the filter output file.  Therefore, this
wizard will prompt for
   1. A filter name
   2. A gate to determine which events will be shipped
      to the filter file.
   3. The set of parameters to write to the filter file.
   4. The name of the filter file, and whether or not
      to enable the filter.

   Ok. Let's get started with the filter name.
}

        label $win.label  -text {Filter Name: }
        entry $win.name   -width 32
        grid $win.info   -
        grid $win.label $win.name
        $self configurelist $args

    }
    # nextOk
    #     Returns {} if it's ok to procede to the next
    #     step of the wizard.. otherwise a message to display
    #
    method nextOk {} {
        if {[$win.name get] == ""} {
            return {Please type a filter name into the entry on the wizard}
        } else {
            return {}
        }
    }
    # configure -filtername name
    #     Set the filter name in the entry widget.
    #
    onconfigure -filtername value {
        setEntry $win.name $value
    }
    #  cget -filtername
    #       Retrieve the filter name from the entry widget.
    #
    oncget -filtername {
        return [$win.name get]
    }
}

# selectFilterGate is a filter wizard that lets the user select the gate
# to apply to the filter.  We use a browser restricted to undtailed gates.
# The -gatescript option is used to select the current gate.
# The gates listed are filtered so that if a gate was selected it will no longer
# show.
#
#   Layout:
#     +-------------------------------------------------+
#     | +----------------------------------------+      |
#     | |   The browser window                   |      |
#     | +----------------------------------------+      |
#     |     {gate selected}                             |
#     +-------------------------------------------------+
snit::widget selectFilterGate {
    option -gatename {}


    variable emptyString {<..........>}

    constructor args {
        message $win.info -text {
Select the gate that determines which
events will be written by the filter
by double clicking it in the window
below.  If you choose the wrong gate
by mistake, just double click the
correct one to replace your initial
choice}

        label $win.gatelbl -text {Gate Chosen: }
        label $win.gate -text $emptyString
        browser $win.b    -width 4in                               \
                          -detail 0                                \
                          -restrict gates                          \
                          -filtergates [mymethod gateFilter]       \
                          -gatescript  [mymethod selectGate]       \
                          -showcolumns type

        grid $win.info      -
        grid $win.b         -
        grid $win.gatelbl   $win.gate
        $self configurelist $args

    }
    # gateFilter descr
    #       Determine which gates are shown in the browser.
    #       If a gate has been selected, it will not be
    #       displayed.
    # Parameters:
    #    descr   - the gate description
    #
    method gateFilter descr {
        set gate [lindex $descr 0]
        set current [$win.gate cget -text]

        if {$gate == $current} {
            return 0
        } else {
            return 1
        }
    }
    # selectGate path
    #       Select a gate, put it in the label and filter it
    #      from the gates tree.
    # path
    #     Path to the gate (includes the leading Gate).
    method selectGate path {
        set name [pathToName $path]
        $win.gate configure -text $name
        $win.b update
    }
    # nextOk
    #     Determines if it's ok to continue to the next state.
    #     If so, returns {} otherwise returns a message describing
    #     why it isn't
    #     We require the user to have selected a gate.
    #
    method nextOk {} {
        if {[$win.gate cget -text] == $emptyString} {
            return {Before you can continue you must select a gate for your filter}
        } else {
            return {}
        }
    }
    # configure -gatename value

    onconfigure -gatename value {
        $win.gate configure -text $value
        $win.b update
    }
    # cget -gatename
    #
    oncget -gatename {
        return [$win.gate cget -text]
    }

}
# selectFilterParameters
#        This wizard stage asks the user to select the set of
#        parameters that will be written for each event that makes the gate.
# Layout:
#     +----------------------------------------+
#     |   Explanatory text                     |
#     | +----------------------+  +----------+ |
#     | |   browser restricted |  |scrolling | |
#     | | to parameters only   |  | listbox  | |
#     | |                      |  | of params| |
#     | |                      |  | chosen   | |
#     | +----------------------+  +----------+ |
#     +----------------------------------------+
# Options:
#    -parameters    when set the initial set of parameters
#                   loaded into the list box.
#                   when gotten the list of parameters in
#                   the list box.
#
snit::widget selectFilterParameters {
    option -parameters

    constructor args {
        message $win.info -justify left -width 4i -text {
Choose the parameters you want written to the filter file when the
filter gate has been made by double-clicking on the parameters in
the left window.  If you made a mistake, you can remove a parameter
from the list of selected parameters in the right window by
double-clicking it.}

        label      $win.paramlabel  -text Parameters:
        listbox    $win.parameters  -yscrollcommand [list $win.paramscroll set]
        scrollbar  $win.paramscroll -orient vertical -command [list $win.parameters yview]

        browser    $win.b    -restrict parameters -detail 0      \
                             -showcolumns {low high bins units}  \
                             -parameterscript [mymethod selectParameter] \
                             -filterparameters [mymethod parameterFilter] \
                             -width 4i

        grid $win.info        -                  - -sticky ew
        grid $win.b           x                  x
        grid   ^              $win.paramlabel
        grid   ^              $win.parameters    $win.paramscroll -sticky ns

        bind $win.parameters <Double-1> +[mymethod removeParameter %x %y]


        $self configurelist $args
    }
    # selectParameter path
    #         Adds the parameter to the list box (set of parameters
    #         in the filter).  After the addition, the browser is
    #         updated as the simplest way to remove the parameter from the
    #         choices.
    # Parameters:
    #    path   - The full path to the parameter. Full path means that it includes the Parameters. prefix.
    #
    method selectParameter path {
        set name [pathToName $path]
        $win.parameters insert end $name
        $win.b update
    }
    # removeParameter x y
    #        Remove a parameter from the list box.
    #        The browser is then updated to ensure that this parameter
    #        is once more available for selection.
    # Parameters:
    #    x,y   - Cooordinates of the double click... This points to
    #            the item that will be removed from the listbox.
    method removeParameter {x y} {
        set index [$win.parameters index @$x,$y]
        $win.parameters delete $index
        $win.b update
    }
    #  parameterFilter descr
    #         This proc is called byt the browser when updating to determine
    #         which parameters are allowed to appear in the browser.
    #         A parameter can appear in the browser only if it is not already
    #         selected (in the listbox).
    # Parameters:
    #    descr  - The SpecTcl parameter description for the guy to check.
    # Returns:
    #    0 - Don't display parameter.
    #    1 - Display parameter.
    #
    method parameterFilter descr {
        set name [lindex $descr 0]
        set selected [$win.parameters get 0 end]

        if {[lsearch -exact $selected $name] == -1} {
            return 1
        } else {
            return 0
        }
    }
    # nextOk
    #     This is called by the wizard framework to verify that it is ok to
    #     continue to the next stage of the wizard.  The user must select
    #     at least one parameter before they can go to the next stage.
    # Returns:
    #   {}    - The wizard can go to the next stage.
    #   other - The  message to display to the user if it is not ok to procede.
    method nextOk {} {
        set selected [$win.parameters get 0 end]
        if {[llength $selected] != 0} {
            return {}
        } else {
            return {
Please select at least one parameter to be output by your filter
for events that make the filter gate true.}
        }
    }
    # configure -parameters list
    #       Called to process setting the -parameters
    #       option.   The list is added to the end of the list box
    #       The browser is also updated to remove these items from
    #       the browser.
    #
    onconfigure -parameters list {
        eval $win.parameters insert end $list
        $win.b update
    }
    # cget -parameters
    #       Called to fetch the list of parameters the
    #       user selected from the listbox.
    #
    oncget      -parameters {
        return [$win.parameters get 0 end]
    }
}
# setFileAndState
#        This filter wizard element asks the user to specify
#        the filter output file and state (enabled/disabled)
#        of the wizard.
# Layout:
#      The file selection box below is from Iwidgets
# +-------------------------------------------------------+
# |   Instructions:                                       |
# | +--------------------------------------+              |
# | | File selection box                   |  [] Enabled  |
# | +--------------------------------------+              |
# +-------------------------------------------------------+
#
# Options:
#   -filepath      - Specifies an initial filepath or gets the current one.
#   -active        - Boolean to specify the initial state or get current state.
#
snit::widget setFileAndState {
    option -filepath
    option -active

    constructor args {
        message $win.instructions -width 4i -justify left -text {
Select the file to which the filtered data will be written.
Filters are either enabled or disabled.   When disabled, filters
are not evaluated.  When enabled, they can write data to their
output files.  If you want the filter to be enabled immediately,
be sure the 'Enabled' Checkbox is checked before clicking the
'Finish' button.}

        iwidgets::fileselectionbox $win.fsbox -directory [pwd]
        $win.fsbox.filter delete 0 end
        setEntry $win.fsbox.filter [file join [pwd] *.flt]
        $win.fsbox filter

        checkbutton $win.active -text Active

        grid $win.instructions -   -sticky ew
        grid $win.fsbox        $win.active -sticky s



        $self configurelist $args
    }
    # nextOk
    #     Determines if it is ok to continue to the next state
    #     of our enclosing wizard.
    # Returns:
    #   {} ok to go on.
    #   other The human readable reason we can't continue.
    method nextOk {} {
        set file [$win.fsbox get]
        if {$file != ""} {
            return {}
        } else {
            return {You must select a filter output file to proceed.}
        }
    }
    # configure -filepath path
    #         Set the current filepath:
    #         The file entry is filled in with path.
    #         The filter is filled in with the
    #          [file join [file dirname $path] *.[file extension $path]]
    #         The file directory is set to [file dirname $path]
    #         The file selection box will then be told to do another filter
    #         pass
    #
    onconfigure -filepath path {
        set directory [file dirname $path]
        set filetype  [file extension $path]

        setEntry $win.fsbox.selection $path
        setEntry $win.fsbox.filter    [file join $directory *.$filetype]
        $win.fsbox  configure -directory $directory
        $win.fsbox filter

    }
    #  confiugure -active bool
    #          Sets or clears the active checkbox according to the value
    #          of bool.
    #
    onconfigure -active   value {
        if {$value} {
            $win.active select
        } else {
            $win.active deselect
        }
    }
    #   cget -filepath
    #         retrieve the file path from the fileselectionbox.
    #
    oncget -filepath {
        return [$win.fsbox get   ]
    }
    # cget -active
    #        Retrieve the state of the active checkbox.  This is
    #        a bit tricky since we have to figure out which variable
    #        holds it.
    oncget -active {
        set var [$win.active cget -variable]
        return [set ::$var]
    }
}
