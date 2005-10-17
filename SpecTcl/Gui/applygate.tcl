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

package provide applygate 1.0
package require snit
package require browser
package require guiutilities
package require guihelp

# Selects a gate to apply to a spectrum.   This
# brings up a browser from which the user can select the
# single gate to apply to the spectrum.
#
# The dialog is laid out as follows:
#
# +--------------------------------------------------------+
# | +------------------------------+                       |
# | | Gate browser                 | Spectrum: <name>      |
# | |                              | Gate: <name>          |
# | +------------------------------+                       |
# |  [Ok]   [Cancel]                                       |
# +--------------------------------------------------------+

snit::widget applyGateOnSpectrum {
    hulltype toplevel
    option   -spectrum
    option   -okcommand
    option   -cancelcommand
    option   -initialgate

    variable emptyString {<..........>}
    variable hidden      {}

    constructor args {
        set $options(-spectrum) $emptyString


        label $win.speclabel -text Spectrum:
        label $win.spectrum  -text $emptyString
        label $win.gatelabel -text Gate:
        label $win.gate      -text $emptyString

        button $win.ok       -text Ok     -command [mymethod onOk]
        button $win.cancel   -text Cancel -command [mymethod onCancel]
        button $win.help     -text Help   -command [list spectclGuiDisplayHelpTopic applyGateOnSpectrum]

        browser $win.browser  -gatescript [mymethod selectGate]       \
                              -restrict   gates  -detail 0            \
                              -showcolumns type                       \
                              -filtergates [mymethod gateFilter]      \
                              -width 4in

        $self configurelist $args

        # layout the widget geometry.

        grid $win.browser         x             x
        grid     ^          $win.speclabel $win.spectrum
        grid     ^          $win.gatelabel $win.gate
        grid     $win.ok    $win.cancel        $win.help
    }

    onconfigure -spectrum name {
        $win.spectrum configure -text $name
        set options(-spectrum) $name
    }
    onconfigure -initialgate name {
        $win.gate configure -text $name
        $win.browser update
        set options(-initialgate) $name
    }
    # onOk
    #     Called in response to the ok button being clicked.
    #
    method onOk {} {
        if {$options(-okcommand) != ""} {
            eval $options(-okcommand)
        }
        if {$hidden != {}} {
            destroy $hidden
            set hidden {}
        }
    }
    # onCancel
    #     Called in response to the cancel button being clicked.
    method onCancel {} {
        $win.gate configure -text $emptyString
        if {$options(-cancelcommand) != ""} {
            eval $options(-cancelcommand)
        }
        if {$hidden != {}} {
            destroy $hidden
            set hidden {}
        }
    }
    # selectGate
    #     Called when a gate is double clicked.
    #     The path is translated to a gate name and loaded into the
    #     gate widget.
    # Parameters:
    #   path  - Path to the gate.
    #
    method selectGate {path} {
        set name [pathToName $path]
        $win.gate configure -text $name
        $win.browser update
    }
    # gateFilter descr
    #       Called to determine which gates should be in the browser.
    # Parameters:
    #    descr  - the SpecTcl description of the gate.
    # Returns:
    #   0    - If the gate should not appear.
    #   1    - If the gate shoulid appear.
    #
    method gateFilter {descr} {
        set gate [$win.gate cget -text]
        if {$gate == $emptyString} {
            return 1
        }
        set name [lindex $descr 0]
        if {$name == $gate} {
            return 0
        } else {
            return 1
        }
    }
    # modal
    #    Makes the dialog modal by forcing it to appear and
    #    doing a grab/tkwait on it.  The window 'hidden'
    #    is used to kill the wait.
    #
    method modal {} {
        focus $win
        set   hidden [frame $win.hidden]
        wm deiconify $win
        grab  $win
        tkwait window $hidden

    }
    # getGate
    #     Returns the contents of the gate label
    #     or "" if it contains emptyString.
    method getGate {} {
        set name [$win.gate cget -text]
        if {$name == $emptyString} {
            return {}
        } else {
            return $name
        }
    }
}
#
#  Dialog to prompt for a set of spectra to which a gate might be applied.
#  Options:
#       -gate          - specifies/gets the gate name.
#       -applications  - specifies/gets a list of spectra to which this gate
#                       should be applied.
#       -okcommand     - Command to execute on the ok click.
#       -cancelcommand - script to execute on the cancel command.
#
# The layout of the dialog is:
#
#   +-----------------------------------------------------------+
#   |  +--------------------------------+     +---------------+ |
#   |  |Spectrum browser                |     | Spectrum list | |
#   |  +--------------------------------+     +---------------+ |
#   |    [Ok}         [Cancel]                                  |
#   +-----------------------------------------------------------+
#
snit::widget applyGateToMultiple {
    hulltype toplevel

    option -gate          {}
    option -applications  {}
    option -okcommand     {}
    option -cancelcommand {}

    variable hidden {}
    variable emptyString {<..........>}

    constructor args {

        label     $win.gatelbl -text Gate:
        label     $win.gate    -text $emptyString

        listbox   $win.spectra              -yscrollcommand [list $win.specscroll set]
        scrollbar $win.specscroll -orient vertical -command [list $win.spectra yview]

        browser $win.browser -detail 0 -restrict {spectra} -showcolumns {type}       \
                             -filterspectra     [mymethod spectrumFilter]            \
                             -spectrumscript    [mymethod selectSpectrum] -width 4in

        button $win.ok     -text Ok     -command [mymethod onOk]
        button $win.cancel -text Cancel -command [mymethod onCancel]
        button $win.help   -text Help   -command [list spectclGuiDisplayHelpTopic applyThisGateOnSpectra]

        $self configurelist $args

        #  Setup widget geometry:

        grid $win.browser     x            x
        grid     ^            $win.gatelbl $win.gate
        grid     ^            $win.spectra $win.specscroll -sticky ns
        grid $win.ok          $win.cancel  $win.help

        $win.browser update
    }
    # spectrumFilter descr
    #       Filter the spectra that appear in the browser.
    # Parameters:
    #   desc   - SpecTcl description of a spectrum.
    method spectrumFilter descr {
        set application [$win.spectra get 0 end]
        set spectrum    [lindex $descr 1]

        if {[lsearch -exact $application $spectrum] != -1} {
            return 0
        }
        return 1

    }
    #  selectSpectrum path
    #       Responds to a double click in the browser.  The spectrum
    #       is added to the application listbox.
    # Parameters:
    #    path   -Path to a spectrum.
    method selectSpectrum path {
        set name [pathToName $path]
        $win.spectra insert end $name

        $win.browser update
    }
    # onOk
    #      Called when the ok button is clicked.
    #      the -okcommand script is invoked if defined.
    #      If it exists, the hidden widget is destroyed.
    #
    method onOk {} {
        if {$options(-okcommand) != ""} {
            eval $options(-okcommand)
        }
        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }
    }
    # onCancel
    #       The Dependencies box is emptied out
    #       and:
    #       If the -cancelcommand script is defined it is run.
    #       If the hidden widget exists it is destroyed.
    #
    method onCancel {} {
        $win.spectra delete 0 end
        if {$options(-cancelcommand) != ""} {
            eval $options(-cancelcommand)
        }
        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }
    }
    # modal
    #     Turn the dialog into an application modal dialog.
    #     The dialog is forced visible, given focus and a grab.
    #     the hidden widget is created, and tkwaited on to indicate
    #     when the dialog exited.
    #
    method modal {} {
        wm deiconify $win
        focus $win
        set hidden [frame $win.hidden]
        grab $win

        tkwait window $hidden

    }
    #   Configuration callbacks:

    # onconfigure -gate value
    #        Sets the gate widget text.
    #
    onconfigure -gate value {
        $win.gate configure -text $value
        set options(-gate) $value
    }
    #  onconfigure -applications value
    #       Sets the list of applications.
    onconfigure -applications values {
        $win.spectra delete 0 end
        foreach dependency $values {
            $win.spectra insert end $dependency
        }
        $win.browser update
        set options(-applications) $values
    }
    #  Configuration retrieval callbacks:
    #

    #  Get the list of spectra from the
    #  widget box.
    #
    oncget   -applications {
        return [$win.spectra get 0 end]
    }

}

# selectGateApplyToMultiple
#         This provides a dialog megawidget that allows the
#         user to select a single gate and apply it to
#         a number of spectra.   This is done by making
#         an object browser instance that contains gates and
#         spectra, letting the user specify both the gate and
#         the set of spectra with it:
# Layout:
#    +------------------------------------------------------------+
#    | +-----------------------------+                            |
#    | | browser                     |  Gate: <gatename>          |
#    | |                             |                            |
#    | |                             |           Spectra          |
#    | |                             |      +-----------------++  |
#    | |                             |      |       ...       ||  |
#    | +-----------------------------+      +-----------------++  |
#    +------------------------------------------------------------+
#    |   <Ok>     <Cancel>                                        |
#    +------------------------------------------------------------+
# Options:
#    -okcommand
#    -cancelcommand
snit::widget selectGateApplyToMultiple {
    hulltype toplevel
    option -okcommand {}
    option -cancelcommand {}

    variable hidden  {}
    variable emptyString {<..........>}

    constructor args {
        label     $win.gatelabel     -text {Gate: }
        label     $win.gate          -text $emptyString

        label     $win.spectrumlabel -text {Spectra:}
        listbox   $win.spectra       -yscrollcommand [list $win.scrollbar set]
        scrollbar $win.scrollbar     -orient vertical -command [list $win.spectra yview]

        browser   $win.browser      -restrict {gates spectra} -detail 0 -showcolumns type \
                                    -gatescript      [mymethod selectGate]                 \
                                    -spectrumscript  [mymethod addSpectrum]                \
                                    -filtergates     [mymethod gateFilter]                 \
                                    -filterspectra   [mymethod spectrumFilter]             \
                                    -width  3in
        frame     $win.command        -relief groove -borderwidth 3
        button    $win.command.ok     -text {Ok}     -command [mymethod onOk]
        button    $win.command.cancel -text {Cancel} -command [mymethod onCancel]
        button    $win.command.help   -text Help     -command [list spectclGuiDisplayHelpTopic selectGateApplyToMultiple]


        grid $win.browser             x              x               x
        grid      ^              $win.gatelabel   $win.gate          x
        grid      ^                   x           $win.spectrumlabel x
        grid      ^                   x           $win.spectra       $win.scrollbar -sticky ns
        pack $win.command.ok      $win.command.cancel $win.command.help -side left
        grid $win.command             -            -                 -  -sticky nesw

        bind $win.spectra <Double-1>  [mymethod removeSpectrum %x %y]

        $self configurelist $args
    }
    # modal
    #     Turn the dialog into a modal dialog
    #
    method modal {} {
        if {$hidden == ""} {
            set hidden [frame $win.hidden]
            wm deiconify $win
            focus $win
            grab  $win
            tkwait window $hidden
            set hidden {}
        }
    }
    # selectGate path
    #   Select a gate from the gate list.. The gate name
    #   is put into the $win.gate widget and the browser updated
    #   to force the filter to act on theis.
    # Parameters:
    #     path - The full path to the item This includes a Gates. path element.
    method selectGate path {
        set gate [pathToName $path]
        $win.gate configure -text $gate
        $win.browser update
    }
    # addSpectrum path
    #       Adds a spectrum to the list of spectra that will be gated on the gate.
    # Parameters:
    #  path   - The path to the spectrum.
    method addSpectrum path {
        set spectrum [pathToName $path]
        $win.spectra insert end $spectrum
        $win.browser update
    }
    # removeSpectreum x y
    #      Removes the spectrum from the list box that is nearest to
    #      the pointer.
    # Parameters:
    #   x,y   - Position of the pointer.
    method removeSpectrum {x y} {
        set index [$win.spectra index @$x,$y]
        $win.spectra delete $index
        $win.browser update
    }
    # onOk
    #     Called when the ok button is clicked.   Call the okcommand
    #     destroy the hidden window if modal.
    #
    method onOk  {} {
        set script $options(-okcommand)
        if {$script != ""} {
            eval $script
        }

        if {$hidden != ""} {
            destroy $hidden
            set hidden ""
        }
    }
    #onCancel
    #      Cancel basically the same as onOk but
    #      - triggered on a cancel click.
    #      - blanks out the data first.
    #
    method onCancel {} {
        $win.gate configure  -text $emptyString
        $win.spectra delete 0 end

        set script $options(-cancelcommand)
        if {$script != ""} {
            eval $script
        }

        if {$hidden != ""} {
            destroy $hidden
            set hidden {}
        }
    }
    # gateFilter descr
    #        Determines if the input gate should be
    #        displayed.   The gate is displayed as long
    #        as it is not the selected gate.
    # Parameters:
    #    descr  - Description of the gate.
    method gateFilter descr {

        set name [lindex $descr 0]
        set selected [$win.gate cget -text]

        if {$name != $selected} {
            return 1
        } else {
            return 0
        }
    }
    # spectrumFilter descr
    #          Determines if the input spectrum should
    #          be displayed in the browser.   All spectra
    #          in the list of selected spectra will not be displayed.
    # Parameters:
    #   descr   - The SpecTcl description of the spectrum.
    #
    method spectrumFilter descr {
        set name [lindex $descr 1]
        set selected [$win.spectra get 0 end]
        if {[lsearch -exact $selected $name] == -1} {
            return 1
        } else {
            return 0
        }

    }
    # getGate
    #       Get the gate name ({} if empty).
    method getGate {} {
        set gate [$win.gate cget -text]
        if {$gate == $emptyString} {
            return {}
        } else {
            return $gate
        }
    }
    # getSpectra
    #     Return the list of spectra in the selected spectrum box.
    #
    method getSpectra {} {
        return [$win.spectra get 0 end]
    }
}


# selectAndApplyGate
#        Selects a gate and several spectra.
#        The gate is applied to the spectra.
#
proc selectAndApplyGate {} {
    selectGateApplyToMultiple .applygatetomultiple
    .applygatetomultiple modal

    if {[winfo exists .applygatetomultiple]} {
        set gate    [.applygatetomultiple getGate]
        set spectra [.applygatetomultiple getSpectra]

        if { ($gate != "") && ([llength $spectra] != 0)} {
            eval apply $gate $spectra

        }
        failsafeWrite
        destroy .applygatetomultiple
    }


}
